// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GlobalShader.h"
#include "ShaderParameters.h"
#include "ShaderParameterStruct.h"
#include "Shader.h"
#include "ShaderCompilerCore.h"
#include "ComputeShaderActor.generated.h"

//-----
//USTRUCT(BlueprintType)
//struct FGlobalShaderStructData
//{
//	GENERATED_USTRUCT_BODY()
//
//	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = ShaderData)
//	FLinearColor ColorOne;
//
//	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = ShaderData)
//	FLinearColor ColorTwo;
//
//	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = ShaderData)
//	FLinearColor ColorThree;
//
//	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = ShaderData)
//	FLinearColor ColorFour;
//
//	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = ShaderData)
//	int32 ColorIndex;
//};
//
//BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FGlobalUniformStructData, )
//	SHADER_PARAMETER(FVector4f, ColorOne)
//	SHADER_PARAMETER(FVector4f, ColorTwo)
//	SHADER_PARAMETER(FVector4f, ColorThree)
//	SHADER_PARAMETER(FVector4f, ColorFour)
//	SHADER_PARAMETER(int32, ColorIndex)
//END_GLOBAL_SHADER_PARAMETER_STRUCT()
//typedef TUniformBufferRef<FGlobalUniformStructData> FGlobalUniformStructDataBufferRef;

//class BOIDS01_API FGlobalComputeTestShader : public FGlobalShader
//{
//	//DECLARE_SHADER_TYPE(FGlobalComputeTestShader, Global);
//	DECLARE_GLOBAL_SHADER(FGlobalComputeTestShader);
//	//SHADER_USE_PARAMETER_STRUCT(FGlobalComputeTestShader, FGlobalShader);
//
//public:
//	FGlobalComputeTestShader() {};
//	FGlobalComputeTestShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
//		: FGlobalShader(Initializer)
//	{
//		OutputSurface.Bind(Initializer.ParameterMap, TEXT("OutputSurface"));
//		TestStructureBufferSurface.Bind(Initializer.ParameterMap, TEXT("TestStructureBuffer"));
//	}
//
//	static bool ShouldCache(EShaderPlatform Platform)
//	{
//		return IsFeatureLevelSupported(Platform, ERHIFeatureLevel::SM5);
//	}
//
//	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
//	{
//		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
//	}
//
//	//virtual bool Serialize(FArchive& Ar) override
//	//{
//	//	bool bShaderHasOutdatedParam = FGlobalShader::Serialize(Ar);
//	//	Ar << OutputSurface << TestStructureBufferSurface;
//	//	return bShaderHasOutdatedParam;
//	//}
//
//	void SetSurface(FRHICommandList& RHICmdList, FUnorderedAccessViewRHIRef OutputSurfaceUAV , FGlobalShaderStructData& ShaderStructData, FUnorderedAccessViewRHIRef& TestStructureBuffUAV)
//	{
//		// RenderTargetTexture
//		//FComputeShaderRHIRef ComputeShaderRHI = GetComputeShader();
//		FComputeShaderRHIRef ComputeShaderRHI = RHICmdList.GetBoundComputeShader();
//		if (OutputSurface.IsBound())
//			RHICmdList.SetUAVParameter(ComputeShaderRHI, OutputSurface.GetBaseIndex(), OutputSurfaceUAV);
//
//		ShaderStructData.ColorOne;
//		// UniformBuffer
//		FGlobalUniformStructData UniformData;
//		UniformData.ColorOne = ShaderStructData.ColorOne;
//		UniformData.ColorTwo = ShaderStructData.ColorTwo;
//		UniformData.ColorThree = ShaderStructData.ColorThree;
//		UniformData.ColorFour = ShaderStructData.ColorFour;
//		UniformData.ColorIndex = ShaderStructData.ColorIndex;
//		SetUniformBufferParameterImmediate(RHICmdList, RHICmdList.GetBoundComputeShader(), GetUniformBufferParameter<FGlobalUniformStructData>(), UniformData);
//
//		// StructuredBuffer
//		if (TestStructureBufferSurface.IsBound())
//			RHICmdList.SetUAVParameter(ComputeShaderRHI, TestStructureBufferSurface.GetUAVIndex(), TestStructureBuffUAV);
//	}
//
//	void UnbindBuffers(FRHICommandList& RHICmdList)
//	{
//		FComputeShaderRHIRef ComputeShaderRHI = RHICmdList.GetBoundComputeShader();
//		if (OutputSurface.IsBound())
//			RHICmdList.SetUAVParameter(ComputeShaderRHI, OutputSurface.GetBaseIndex(), FUnorderedAccessViewRHIRef());
//
//		if (TestStructureBufferSurface.IsBound())
//			RHICmdList.SetUAVParameter(ComputeShaderRHI, TestStructureBufferSurface.GetUAVIndex(), FUnorderedAccessViewRHIRef());
//	}
//
//private:
//	FShaderResourceParameter OutputSurface;
//	FRWShaderParameter TestStructureBufferSurface;
//};

//IMPLEMENT_GLOBAL_SHADER(FGlobalComputeTestShader, "/ComputeShaderPlugin/GlobalShaderTest.usf", "MainCS", SF_Compute);
//IMPLEMENT_SHADER_TYPE(, FGlobalComputeTestShader, TEXT("/ComputeShaderPlugin/GlobalShaderTest.usf"), TEXT("MainCS"), SF_Compute);

//-----

UCLASS()
class PROJECTDS_API AComputeShaderActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AComputeShaderActor();

	FBufferRHIRef _newDirectionsBuffer;
	FUnorderedAccessViewRHIRef _newDirectionsBufferUAV;

	FBufferRHIRef _TestStructureBuffer;
	FUnorderedAccessViewRHIRef _TestStructureBufferUAV;

	int32 iNum = 1000;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void OnDestroyedActor(AActor* DestroyedActor);

};
