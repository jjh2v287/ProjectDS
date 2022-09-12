// Fill out your copyright notice in the Description page of Project Settings.


#include "ComputeShaderActor.h"

#include "ShaderParameterUtils.h"
#include "RHIStaticStates.h"
#include "Shader.h"
#include "GlobalShader.h"
#include "RenderGraphBuilder.h"
#include "ShaderParameterStruct.h"
#include "UniformBuffer.h"
#include "RHICommandList.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"

/*
 Project.uproject 파일에서
 "Modules": [
		{
			"Name": "ProjectDS",
			"Type": "Runtime",
			"LoadingPhase": "Default",
			"AdditionalDependencies": [
				"Engine"
			]
		}
	],

"LoadingPhase": "Default", 부분을 "LoadingPhase": "PostConfigInit"로 변경해야 정상적으로 실행된다
*/

class FMyTestCS : public FGlobalShader
{
	//DECLARE_GLOBAL_SHADER(FMyTestCS)
	//DECLARE_EXPORTED_SHADER_TYPE(FMyTestVS, Global, /*MYMODULE_API*/);

	DECLARE_GLOBAL_SHADER(FMyTestCS);
	SHADER_USE_PARAMETER_STRUCT(FMyTestCS, FGlobalShader);

	//FMyTestCS() { }
	//FMyTestCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
	//	: FGlobalShader(Initializer)
	//{
	//	//TestStructureBufferSurface.Bind(Initializer.ParameterMap, TEXT("TestStructureBuffer"));
	//}

	static bool ShouldCache(EShaderPlatform Platform)
	{
		return true;
	}
public:
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER(float, dt)
		SHADER_PARAMETER(float, totalTime)

		SHADER_PARAMETER_UAV(RWStructuredBuffer<float3>, newDirections)
		SHADER_PARAMETER_UAV(RWStructuredBuffer<float3>, TestStructureBuffer)
	END_SHADER_PARAMETER_STRUCT()

	//FRWShaderParameter TestStructureBufferSurface;
};

IMPLEMENT_GLOBAL_SHADER(FMyTestCS, "/ComputeShaderPlugin/MyTest.usf", "MainCS", SF_Compute);
//IMPLEMENT_SHADER_TYPE(, FMyTestCS, TEXT("/Shaders/ComputeShaderPlugin/MyTest.usf"), TEXT("MainCS"), SF_Compute);

// Sets default values
AComputeShaderActor::AComputeShaderActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AComputeShaderActor::BeginPlay()
{
	Super::BeginPlay();

	//this->OnDestroyed.Clear();
	//if(!this->OnDestroyed.IsBound())
	//	this->OnDestroyed.AddDynamic(this, &AComputeShaderActor::OnDestroyedActor);

	TResourceArray<FVector3f> resourceArray;
	const FVector3f zero(0.0f);

	resourceArray.Init(zero, iNum);
	resourceArray.SetAllowCPUAccess(true);

	
	const size_t size = sizeof(FVector3f);
	FRHIResourceCreateInfo createInfo(TEXT("newDirections"));
	
	{
		createInfo.ResourceArray = &resourceArray;

		_newDirectionsBuffer = RHICreateStructuredBuffer(size, size * iNum, BUF_UnorderedAccess | BUF_ShaderResource, createInfo);
		_newDirectionsBufferUAV = RHICreateUnorderedAccessView(_newDirectionsBuffer, true, false);
		//_newDirectionsBufferUAV = RHICreateUnorderedAccessView(_newDirectionsBuffer, PF_A32B32G32R32F);
	}

	{
		resourceArray.Init(FVector3f(100.f, 100.f, 100.f), iNum);

		createInfo.ResourceArray = &resourceArray;
		_TestStructureBuffer = RHICreateStructuredBuffer(size, size * iNum, BUF_UnorderedAccess | BUF_ShaderResource, createInfo);
		_TestStructureBufferUAV = RHICreateUnorderedAccessView(_TestStructureBuffer, true, false);
	}
}

void AComputeShaderActor::OnDestroyedActor(AActor* DestroyedActor)
{
	if(_newDirectionsBuffer.IsValid())
		_newDirectionsBuffer.SafeRelease();
	if (_newDirectionsBufferUAV.IsValid())
		_newDirectionsBufferUAV.SafeRelease();

	if (_TestStructureBuffer.IsValid())
		_TestStructureBuffer.SafeRelease();
	if (_TestStructureBufferUAV.IsValid())
		_TestStructureBufferUAV.SafeRelease();
}

static FIntVector groupSize(int numElements)
{
	const int threadCount = 4;

	int count = ((numElements - 1) / threadCount) + 1;

	return FIntVector(count, 1, 1);
}

// Called every frame
void AComputeShaderActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float totalTime = GetWorld()->TimeSeconds;
	const float dt = FMath::Min(1.0f / 60.0f, DeltaTime);

	ENQUEUE_RENDER_COMMAND(FComputeShaderTest01)(
		[&, totalTime, dt](FRHICommandListImmediate& RHICommands)
		{
			//FRHICommandListImmediate& RHICmdList = FRHICommandListExecutor::GetImmediateCommandList();
			FRDGBuilder GraphBuilder(RHICommands);

			FMyTestCS::FParameters* Parameters = GraphBuilder.AllocParameters<FMyTestCS::FParameters>();
			Parameters->dt = dt;
			Parameters->totalTime = totalTime;
			Parameters->newDirections = _newDirectionsBufferUAV;
			Parameters->TestStructureBuffer = _TestStructureBufferUAV;

			TShaderMapRef<FMyTestCS> computeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

			/*
			AddPass -> AddPassInternal -> FRDGPass생성시 OverridePassFlags함수에서 AsyncCompute가 되어야하는데 안됨
			관련 변수는
			GRDGAsyncCompute
			LambdaPassType::kSupportsAsyncCompute
			const ERDGPassFlags Flags는 AsyncCompute가 되어야하고
			const ERHIPipeline Pipeline는 AsyncCompute가 되어야한다
			*/
			GraphBuilder.AddPass(
				RDG_EVENT_NAME("FComputeShaderTest01"),
				Parameters,
				ERDGPassFlags::AsyncCompute,
				[Parameters, computeShader, this](FRHIComputeCommandList& RHICmdList)
				{
					TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("Memcpy FComputeShaderTest01 - 00"));
					bool sdfe = RHICmdList.IsAsyncCompute();
					ERHIPipeline fdf = RHICmdList.GetPipeline();
					if (fdf == ERHIPipeline::AsyncCompute)
					{
						int werwe = 0;
						int fdfe = werwe;
					}
					FComputeShaderUtils::Dispatch(RHICmdList, computeShader, *Parameters, groupSize(iNum));

					TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("Memcpy FComputeShaderTest01 - 01"));
					TArray<FVector3f> Data;
					Data.Reset(iNum);
					Data.Init(FVector3f::ZeroVector, iNum);

					TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("Memcpy FComputeShaderTest01 - 02"));// RHILockBuffer이 비용일 제일 비싸다
					//FVector3f* SrcPtr = (FVector3f*)RHILockBuffer(_newDirectionsBuffer, 0, sizeof(FVector3f) * iNum, RLM_ReadOnly);
					TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("Memcpy FComputeShaderTest01 - 03"));
					//FMemory::Memcpy(Data.GetData(), SrcPtr, sizeof(FVector3f) * iNum);
					TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("Memcpy FComputeShaderTest01 - 04"));

					//RHIUnlockBuffer(_newDirectionsBuffer);
				});

			GraphBuilder.Execute();
		}
	);

	/*
	ENQUEUE_RENDER_COMMAND(FComputeShaderTest02)(
		[&, totalTime, dt](FRHICommandListImmediate& RHICommands)
		{
			//FRHICommandListImmediate& RHICmdList = FRHICommandListExecutor::GetImmediateCommandList();
			FRDGBuilder GraphBuilder(RHICommands);

			FMyTestCS::FParameters* Parameters = GraphBuilder.AllocParameters<FMyTestCS::FParameters>();
			Parameters->dt = dt;
			Parameters->totalTime = totalTime;
			Parameters->newDirections = _newDirectionsBufferUAV;
			Parameters->TestStructureBuffer = _TestStructureBufferUAV;

			TShaderMapRef<FMyTestCS> computeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

			//AddPass -> AddPassInternal -> FRDGPass생성시 OverridePassFlags함수에서 AsyncCompute가 되어야하는데 안됨
			//관련 변수는
			//GRDGAsyncCompute
			//LambdaPassType::kSupportsAsyncCompute
			//const ERDGPassFlags Flags는 AsyncCompute가 되어야하고
			//const ERHIPipeline Pipeline는 AsyncCompute가 되어야한다
			GraphBuilder.AddPass(
				RDG_EVENT_NAME("FComputeShaderTest02"),
				Parameters,
				ERDGPassFlags::AsyncCompute,
				[Parameters, computeShader, this](FRHIComputeCommandList& RHICmdList)
				{
					bool sdfe = RHICmdList.IsAsyncCompute();
					ERHIPipeline fdf = RHICmdList.GetPipeline();
					if (fdf == ERHIPipeline::AsyncCompute)
					{
						int werwe = 0;
						int fdfe = werwe;
					}
					FComputeShaderUtils::Dispatch(RHICmdList, computeShader, *Parameters, groupSize(iNum));

					TArray<FVector3f> Data;
					Data.Reset(iNum);
					Data.Init(FVector3f::ZeroVector, iNum);

					TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("Memcpy FComputeShaderTest02"));
					FVector3f* SrcPtr = (FVector3f*)RHILockBuffer(_newDirectionsBuffer, 0, sizeof(FVector3f) * iNum, RLM_ReadOnly);
					FMemory::Memcpy(Data.GetData(), SrcPtr, sizeof(FVector3f) * iNum);
					RHIUnlockBuffer(_newDirectionsBuffer);
				});

			GraphBuilder.Execute();
		}
	);*/

	/*
	ENQUEUE_RENDER_COMMAND(FComputeShaderRunner)(
		[&, totalTime, dt](FRHICommandListImmediate& RHICommands)
		{
			QUICK_SCOPE_CYCLE_COUNTER(STAT_UComputeShaderTestComponent_TickComponent);

			FMyTestCS::FParameters parameters;
			parameters.dt = dt;
			parameters.totalTime = totalTime;
			parameters.newDirections = _newDirectionsBufferUAV;
			parameters.TestStructureBuffer = _TestStructureBufferUAV;

			TShaderMapRef<FMyTestCS> computeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
			FComputeShaderUtils::Dispatch(
				RHICommands,
				computeShader,
				parameters,
				groupSize(iNum)
			);

			{
				TArray<FVector3f> Data;
				Data.Reset(iNum);
				Data.Init(FVector3f::ZeroVector, iNum);

				//FVector3f* SrcPtr = (FVector3f*)RHILockBuffer(_newDirectionsBuffer, 0, sizeof(FVector3f) * iNum, RLM_ReadOnly);
				//FMemory::Memcpy(Data.GetData(), SrcPtr, sizeof(FVector3f) * iNum);
				//RHIUnlockBuffer(_newDirectionsBuffer);
				//return Data[0];
			}
		}
	);*/
}

