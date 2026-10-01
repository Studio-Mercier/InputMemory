#include "DeusGoLookViewExtension.h"

#include "Interfaces/IPluginManager.h"
#include "Misc/CoreDelegates.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
#include "SceneViewExtension.h"
#include "ShaderCore.h"

class FDeusGoLookModule : public IModuleInterface
{
public:
	virtual void StartupModule() override
	{
		const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("DeusGoLook"));
		if (Plugin.IsValid())
		{
			AddShaderSourceDirectoryMapping(TEXT("/Plugin/DeusGoLook"), FPaths::Combine(Plugin->GetBaseDir(), TEXT("Shaders")));
		}

		// extensions can only be created once the engine exists
		FCoreDelegates::GetOnPostEngineInit().AddRaw(this, &FDeusGoLookModule::OnPostEngineInit);
	}

	virtual void ShutdownModule() override
	{
		FCoreDelegates::GetOnPostEngineInit().RemoveAll(this);
		ViewExtension.Reset();
	}

private:
	void OnPostEngineInit()
	{
		ViewExtension = FSceneViewExtensions::NewExtension<FDeusGoLookViewExtension>();
	}

	TSharedPtr<FDeusGoLookViewExtension, ESPMode::ThreadSafe> ViewExtension;
};

IMPLEMENT_MODULE(FDeusGoLookModule, DeusGoLook)
