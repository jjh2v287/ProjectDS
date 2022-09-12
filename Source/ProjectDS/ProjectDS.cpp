// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectDS.h"
#include "Modules/ModuleManager.h"

class FProjectDSModule : public IModuleInterface
{
	virtual bool IsGameModule() const override
	{
		return true;
	}

	void StartupModule() override {
		FString ShaderDirectory = FPaths::Combine(FPaths::ProjectDir(), TEXT("Shaders"));
		AddShaderSourceDirectoryMapping("/ComputeShaderPlugin", ShaderDirectory);
	}
};

IMPLEMENT_PRIMARY_GAME_MODULE(FProjectDSModule, ProjectDS, "ProjectDS" );
