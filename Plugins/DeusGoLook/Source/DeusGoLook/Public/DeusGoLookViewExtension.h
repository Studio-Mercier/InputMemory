#pragma once

#include "CoreMinimal.h"
#include "DeusGoLookTypes.h"
#include "SceneViewExtension.h"

class DEUSGOLOOK_API FDeusGoLookViewExtension : public FSceneViewExtensionBase
{
public:
	FDeusGoLookViewExtension(const FAutoRegister& AutoRegister);

	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override {}
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override {}
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override;

	// 5.4+ signature, older engines drop the FSceneView parameter
	virtual void SubscribeToPostProcessingPass(EPostProcessingPass PassId, const FSceneView& View, FAfterPassCallbackDelegateArray& InOutPassCallbacks, bool bIsPassEnabled) override;

private:
	FScreenPassTexture BeforeUpscale_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessMaterialInputs& Inputs);
	FScreenPassTexture AfterTonemap_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessMaterialInputs& Inputs);

	// render thread only, set per family by a command queued ahead of that family's render
	FDeusGoLookStyle RenderStyle;
	float RenderSplit = 0.f;
	bool bRenderEnabled = false;
};
