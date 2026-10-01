#include "DeusGoLookViewExtension.h"

#include "DeusGoLookSettings.h"
#include "PostProcess/PostProcessMaterialInputs.h"
#include "RenderGraphUtils.h"
#include "SceneView.h"
#include "ScreenPass.h"
#include "ShowFlags.h"

// viewport Show > Post Processing > Deus Go Look, also ShowFlag.DeusGoLook in the console
static TCustomShowFlag<> ShowDeusGoLook(TEXT("DeusGoLook"), true, SFG_PostProcess, NSLOCTEXT("DeusGoLook", "ShowFlag", "Deus Go Look"));

class FDeusGoLookPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FDeusGoLookPS);
	SHADER_USE_PARAMETER_STRUCT(FDeusGoLookPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
		SHADER_PARAMETER_RDG_TEXTURE(Texture2D, InputTexture)
		SHADER_PARAMETER_SAMPLER(SamplerState, InputSampler)
		SHADER_PARAMETER_STRUCT_INCLUDE(FSceneTextureShaderParameters, SceneTextures)
		SHADER_PARAMETER(FVector2f, InputTexelSize)
		SHADER_PARAMETER(FLinearColor, FogColor)
		SHADER_PARAMETER(FLinearColor, BackgroundColor)
		SHADER_PARAMETER(FLinearColor, SkyTopColor)
		SHADER_PARAMETER(FLinearColor, MistColor)
		SHADER_PARAMETER(FLinearColor, ShadowTint)
		SHADER_PARAMETER(FLinearColor, HighlightTint)
		SHADER_PARAMETER(FLinearColor, OutlineColor)
		SHADER_PARAMETER(FLinearColor, CreaseColor)
		SHADER_PARAMETER_ARRAY(FVector4f, NeonColors, [4])
		SHADER_PARAMETER(float, FogStart)
		SHADER_PARAMETER(float, FogRange)
		SHADER_PARAMETER(float, FogMaxOpacity)
		SHADER_PARAMETER(float, FogCurve)
		SHADER_PARAMETER(float, BackgroundDistance)
		SHADER_PARAMETER(float, BackgroundOpacity)
		SHADER_PARAMETER(float, SkyGradient)
		SHADER_PARAMETER(float, SkyGradientCurve)
		SHADER_PARAMETER(float, MistOpacity)
		SHADER_PARAMETER(float, MistTop)
		SHADER_PARAMETER(float, MistFalloff)
		SHADER_PARAMETER(float, Desaturation)
		SHADER_PARAMETER(float, DistanceDesaturation)
		SHADER_PARAMETER(float, Contrast)
		SHADER_PARAMETER(float, Vignette)
		SHADER_PARAMETER(float, OutlineStrength)
		SHADER_PARAMETER(float, CreaseStrength)
		SHADER_PARAMETER(float, OutlineThickness)
		SHADER_PARAMETER(float, OutlineDepthThreshold)
		SHADER_PARAMETER(float, CreaseNormalThreshold)
		SHADER_PARAMETER(float, EdgeSoftness)
		SHADER_PARAMETER(float, EdgeFadeDistance)
		SHADER_PARAMETER(float, NeonEnabled)
		SHADER_PARAMETER(float, NeonGlow)
		SHADER_PARAMETER(float, NeonWidth)
		SHADER_PARAMETER(float, NeonFill)
		SHADER_PARAMETER_ARRAY(FVector4f, NeonZones, [4])
		SHADER_PARAMETER(float, NeonDashLength)
		SHADER_PARAMETER(float, NeonDashSpeed)
		SHADER_PARAMETER(float, OutlineFarWidth)
		SHADER_PARAMETER(float, OutlineFromSurface)
		SHADER_PARAMETER(float, OutlineSurfaceDarkness)
		SHADER_PARAMETER(FVector3f, FogNoiseOrigin)
		SHADER_PARAMETER(float, FogNoiseAmount)
		SHADER_PARAMETER(float, FogNoiseScale)
		SHADER_PARAMETER(float, FogNoiseSpeed)
		SHADER_PARAMETER(float, CompareSplit)
		SHADER_PARAMETER(float, AccentHue)
		SHADER_PARAMETER(float, AccentRange)
		SHADER_PARAMETER(float, AccentStrength)
		SHADER_PARAMETER(FVector3f, NeonHexOrigin)
		SHADER_PARAMETER(float, NeonHexSize)
		SHADER_PARAMETER(float, NeonHexStrength)
		SHADER_PARAMETER(float, NeonHexLine)
		SHADER_PARAMETER(float, PatternsEnabled)
		SHADER_PARAMETER_ARRAY(FVector4f, PatternOrigins, [4])
		SHADER_PARAMETER_ARRAY(FVector4f, PatternShapes, [4])
		SHADER_PARAMETER_ARRAY(FVector4f, PatternGrooves, [4])
		SHADER_PARAMETER_ARRAY(FVector4f, PatternTints, [4])
		SHADER_PARAMETER_ARRAY(FVector4f, PatternBevels, [4])
		SHADER_PARAMETER_ARRAY(FVector4f, PatternMotion, [4])
		SHADER_PARAMETER_ARRAY(FVector4f, PatternWaves, [4])
		SHADER_PARAMETER(float, NeonOccludedOpacity)
		SHADER_PARAMETER(float, NeonIgnoreFog)
		SHADER_PARAMETER(float, NeonSaturationBoost)
		SHADER_PARAMETER(float, PosterizeLevels)
		SHADER_PARAMETER(float, PosterizeStrength)
		SHADER_PARAMETER(float, TiltShiftFocus)
		SHADER_PARAMETER(float, TiltShiftWidth)
		SHADER_PARAMETER(float, TiltShiftBlur)
		SHADER_PARAMETER(float, Grain)
		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
};

// same parameter struct, different entry point
class FDeusGoLookDepthPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FDeusGoLookDepthPS);
	using FParameters = FDeusGoLookPS::FParameters;
	SHADER_USE_PARAMETER_STRUCT(FDeusGoLookDepthPS, FGlobalShader);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
};

IMPLEMENT_GLOBAL_SHADER(FDeusGoLookPS, "/Plugin/DeusGoLook/Private/DeusGoLook.usf", "MainPS", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FDeusGoLookDepthPS, "/Plugin/DeusGoLook/Private/DeusGoLook.usf", "DepthPS", SF_Pixel);

FDeusGoLookViewExtension::FDeusGoLookViewExtension(const FAutoRegister& AutoRegister)
	: FSceneViewExtensionBase(AutoRegister)
{
}

void FDeusGoLookViewExtension::BeginRenderViewFamily(FSceneViewFamily& InViewFamily)
{
	const UDeusGoLookSettings* Settings = GetDefault<UDeusGoLookSettings>();
	const bool bEnabled = Settings->bEnabled && ShowDeusGoLook.IsEnabled(InViewFamily.EngineShowFlags);
	const float Split = GIsEditor ? Settings->CompareSplit : 0.f;

	// several viewports render per frame with their own show flags, a member written here would race
	ENQUEUE_RENDER_COMMAND(DeusGoLookParams)([this, bEnabled, Split, Style = Settings->Style](FRHICommandListImmediate&)
	{
		bRenderEnabled = bEnabled;
		RenderSplit = Split;
		RenderStyle = Style;
	});
}

void FDeusGoLookViewExtension::SubscribeToPostProcessingPass(EPostProcessingPass PassId, const FSceneView& View, FAfterPassCallbackDelegateArray& InOutPassCallbacks, bool bIsPassEnabled)
{
	if (!bRenderEnabled)
	{
		return;
	}

	// BeforeDOF is pre TSR, color and depth share resolution and jitter
	// anything reading depth after TSR flickers at depth breaks, fog included
	if (PassId == EPostProcessingPass::BeforeDOF)
	{
		InOutPassCallbacks.Add(FAfterPassCallbackDelegate::CreateRaw(this, &FDeusGoLookViewExtension::BeforeUpscale_RenderThread));
		return;
	}

	if (PassId == EPostProcessingPass::Tonemap)
	{
		InOutPassCallbacks.Add(FAfterPassCallbackDelegate::CreateRaw(this, &FDeusGoLookViewExtension::AfterTonemap_RenderThread));
	}
}

// toggles fold into amounts here so the shader never branches on a bool it could read as a strength
static void FillParameters(FDeusGoLookPS::FParameters& Out, const FDeusGoLookStyle& S, float Split, const FSceneView& View, FIntPoint InputExtent)
{
	const FDeusGoLookFog& Fog = S.Fog;
	const FDeusGoLookBackground& Bg = S.Background;
	const FDeusGoLookMist& Mist = S.Mist;
	const FDeusGoLookGrade& Grade = S.Grade;
	const FDeusGoLookEdges& Edges = S.Edges;
	const FDeusGoLookNeon& Neon = S.Neon;
	const FDeusGoLookStylize& Sty = S.Stylize;

	Out.InputTexelSize = FVector2f(1.f / FMath::Max(InputExtent.X, 1), 1.f / FMath::Max(InputExtent.Y, 1));

	Out.FogColor = Fog.FogColor * Fog.FogIntensity;
	Out.FogStart = Fog.FogStart;
	Out.FogRange = FMath::Max(Fog.FogRange, 1.f);
	Out.FogMaxOpacity = Fog.bFog ? Fog.FogMaxOpacity : 0.f;
	Out.FogCurve = FMath::Max(Fog.FogCurve, 0.25f);
	Out.DistanceDesaturation = Fog.DistanceDesaturation;

	// noise wraps every 64 cells, so the camera offset folds into that period and stays precise with large worlds
	const double NoiseScale = FMath::Max(Fog.FogNoiseScale, 100.f);
	const double NoisePeriod = NoiseScale * 64.0;
	const FVector NoiseOrigin = -View.ViewMatrices.GetPreViewTranslation();
	Out.FogNoiseOrigin = FVector3f(
		float(FMath::Fmod(NoiseOrigin.X, NoisePeriod)),
		float(FMath::Fmod(NoiseOrigin.Y, NoisePeriod)),
		float(FMath::Fmod(NoiseOrigin.Z, NoisePeriod)));
	Out.FogNoiseAmount = Fog.bFogNoise ? Fog.FogNoiseAmount : 0.f;
	Out.FogNoiseScale = float(NoiseScale);
	Out.FogNoiseSpeed = Fog.FogNoiseSpeed;

	Out.BackgroundColor = Bg.BackgroundColor * Bg.BackgroundIntensity;
	Out.SkyTopColor = Bg.SkyTopColor * Bg.BackgroundIntensity;
	Out.BackgroundDistance = FMath::Max(Bg.BackgroundDistance, 1000.f);
	Out.BackgroundOpacity = Bg.bBackground ? 1.f : 0.f;
	Out.SkyGradient = Bg.bSkyGradient ? 1.f : 0.f;
	Out.SkyGradientCurve = FMath::Max(Bg.SkyGradientCurve, 0.25f);

	// shader compares against translated world z, same space SvPositionToTranslatedWorld returns
	Out.MistColor = Mist.MistColor * Mist.MistIntensity;
	Out.MistOpacity = Mist.bMist ? Mist.MistOpacity : 0.f;
	Out.MistTop = float(Mist.MistHeight + View.ViewMatrices.GetPreViewTranslation().Z);
	Out.MistFalloff = FMath::Max(Mist.MistFalloff, 1.f);

	Out.Desaturation = Grade.Desaturation;
	Out.ShadowTint = Grade.ShadowTint;
	Out.HighlightTint = Grade.HighlightTint;
	Out.Contrast = Grade.Contrast;
	Out.Vignette = Grade.Vignette;

	// hue only, 0..1 around the wheel
	Out.AccentHue = Grade.AccentColor.LinearRGBToHSV().R / 360.f;
	Out.AccentRange = FMath::Max(Grade.AccentRange, 0.01f);
	Out.AccentStrength = Grade.bAccent ? Grade.AccentStrength : 0.f;

	Out.OutlineStrength = Edges.bOutlines ? Edges.OutlineStrength : 0.f;
	Out.OutlineColor = Edges.OutlineColor;
	Out.OutlineDepthThreshold = Edges.OutlineDepthThreshold;
	Out.CreaseStrength = Edges.bCreases ? Edges.CreaseStrength : 0.f;
	Out.CreaseColor = Edges.CreaseColor;
	Out.CreaseNormalThreshold = Edges.CreaseNormalThreshold;
	Out.OutlineThickness = Edges.OutlineThickness;
	Out.EdgeSoftness = FMath::Max(Edges.EdgeSoftness, 0.05f);
	Out.EdgeFadeDistance = Edges.EdgeFadeDistance;
	Out.OutlineFarWidth = Edges.OutlineFarWidth;
	Out.OutlineFromSurface = Edges.bOutlineFromSurface ? 1.f : 0.f;
	Out.OutlineSurfaceDarkness = Edges.OutlineSurfaceDarkness;

	const FDeusGoLookNeonZone* Zones[4] = { &Neon.Zone1, &Neon.Zone2, &Neon.Zone3, &Neon.Zone4 };
	for (int32 Index = 0; Index < 4; ++Index)
	{
		const FDeusGoLookNeonZone& Zone = *Zones[Index];
		Out.NeonColors[Index] = FVector4f(Zone.Color.R, Zone.Color.G, Zone.Color.B, Zone.Brightness);
		Out.NeonZones[Index] = FVector4f(Zone.PulseAmount, Zone.PulseSpeed, float(uint8(Zone.LineStyle)), 0.f);
	}
	Out.NeonEnabled = Neon.bNeon ? 1.f : 0.f;
	Out.NeonGlow = Neon.NeonGlow;
	Out.NeonWidth = FMath::Max(Neon.NeonWidth, 1.f);
	Out.NeonFill = Neon.NeonFill;
	Out.NeonDashLength = FMath::Max(Neon.NeonDashLength, 2.f);

	// hex rows repeat every 1.75 cells, so 7 cells tile on every axis and the camera offset can fold into that
	const double HexSize = FMath::Max(Neon.NeonHexSize, 5.f);
	const double HexPeriod = HexSize * 7.0 * 64.0;
	const FVector HexOrigin = -View.ViewMatrices.GetPreViewTranslation();
	Out.NeonHexOrigin = FVector3f(
		float(FMath::Fmod(HexOrigin.X, HexPeriod)),
		float(FMath::Fmod(HexOrigin.Y, HexPeriod)),
		float(FMath::Fmod(HexOrigin.Z, HexPeriod)));
	Out.NeonHexSize = float(HexSize);
	Out.NeonHexStrength = Neon.bNeonHex ? Neon.NeonHexStrength : 0.f;
	Out.NeonHexLine = Neon.NeonHexLine;
	Out.NeonDashSpeed = Neon.NeonDashSpeed;
	Out.NeonOccludedOpacity = Neon.bNeonShowOccluded ? Neon.NeonOccludedOpacity : 0.f;
	Out.NeonIgnoreFog = Neon.bNeonIgnoreFog ? 1.f : 0.f;
	// grade lerps toward luma by Desaturation after tonemap, stretch chroma now so it lands back near the picked color
	Out.NeonSaturationBoost = Neon.bNeonKeepSaturated ? 1.f / FMath::Max(1.f - Grade.Desaturation, 0.15f) : 1.f;

	const FDeusGoLookPatterns& Patterns = S.Patterns;
	const FDeusGoLookPatternSlot* Slots[4] = { &Patterns.Slot5, &Patterns.Slot6, &Patterns.Slot7, &Patterns.Slot8 };
	Out.PatternsEnabled = Patterns.bPatterns ? 1.f : 0.f;
	for (int32 Index = 0; Index < 4; ++Index)
	{
		const FDeusGoLookPatternSlot& Slot = *Slots[Index];

		// same 7 cell fold as the hex fill, triangle rows are 0.875 high for the same reason
		const double Size = FMath::Max(Slot.Size, 5.f);
		const double Period = Size * 7.0 * 64.0;
		const FVector Origin = -View.ViewMatrices.GetPreViewTranslation();
		Out.PatternOrigins[Index] = FVector4f(
			float(FMath::Fmod(Origin.X, Period)),
			float(FMath::Fmod(Origin.Y, Period)),
			float(FMath::Fmod(Origin.Z, Period)),
			0.f);
		Out.PatternShapes[Index] = FVector4f(Slot.Shape == EDeusGoLookPattern::Hexagons ? 1.f : 0.f, float(Size), Slot.FacetVariation, Slot.GrooveWidth);
		Out.PatternGrooves[Index] = FVector4f(Slot.GrooveDarkness, Slot.BevelStrength, Slot.bEdgeGrooves ? 1.f : 0.f, FMath::Max(Slot.WaveSpacing, 2.f));

		// scroll goes in as panels per second so the shader adds it straight to the cell coordinates
		const float Angle = FMath::DegreesToRadians(Slot.MotionAngle);
		const FVector2f Direction(FMath::Cos(Angle), FMath::Sin(Angle));
		const float ScrollCells = Slot.ScrollSpeed / float(Size);
		Out.PatternMotion[Index] = FVector4f(Direction.X * ScrollCells, Direction.Y * ScrollCells, Slot.ShimmerAmount, Slot.ShimmerSpeed);
		Out.PatternWaves[Index] = FVector4f(Slot.WaveStrength, Slot.WaveSpeed, Direction.X, Direction.Y);
		Out.PatternTints[Index] = FVector4f(Slot.Tint);
		Out.PatternBevels[Index] = FVector4f(Slot.BevelColor);
	}

	Out.PosterizeLevels = float(FMath::Max(Sty.PosterizeLevels, 2));
	Out.PosterizeStrength = Sty.bPosterize ? Sty.PosterizeStrength : 0.f;
	Out.TiltShiftFocus = Sty.TiltShiftFocus;
	Out.TiltShiftWidth = Sty.TiltShiftWidth;
	Out.TiltShiftBlur = Sty.bTiltShift ? Sty.TiltShiftBlur : 0.f;
	Out.Grain = Sty.bGrain ? Sty.GrainAmount : 0.f;

	Out.CompareSplit = Split;
}

template <typename TShaderClass>
static FScreenPassTexture RunPass(
	FRDGBuilder& GraphBuilder,
	const FSceneView& View,
	const FPostProcessMaterialInputs& Inputs,
	const FDeusGoLookStyle& Style,
	float Split,
	const TCHAR* PassName)
{
	// 5.3 and older: FScreenPassTexture SceneColor(Inputs.GetInput(EPostProcessMaterialInput::SceneColor));
	const FScreenPassTexture SceneColor = FScreenPassTexture::CopyFromSlice(GraphBuilder, Inputs.GetInput(EPostProcessMaterialInput::SceneColor));
	if (!SceneColor.IsValid())
	{
		return SceneColor;
	}

	// last pass of the chain must draw into the view family target, a texture of our own is dropped and PIE goes black
	// editor viewports composite gizmos after us so they never hit this, Play does
	FScreenPassRenderTarget Output = Inputs.OverrideOutput;
	if (!Output.IsValid())
	{
		// explicit desc, copying SceneColor's carries flags D3D refuses on this RTV
		const FRDGTextureDesc OutputDesc = FRDGTextureDesc::Create2D(
			SceneColor.Texture->Desc.Extent,
			SceneColor.Texture->Desc.Format,
			FClearValueBinding::Black,
			TexCreate_RenderTargetable | TexCreate_ShaderResource);
		FRDGTextureRef OutputTexture = GraphBuilder.CreateTexture(OutputDesc, TEXT("DeusGoLookOutput"));
		Output = FScreenPassRenderTarget(OutputTexture, SceneColor.ViewRect, ERenderTargetLoadAction::ENoAction);
	}

	typename TShaderClass::FParameters* Parameters = GraphBuilder.AllocParameters<typename TShaderClass::FParameters>();
	Parameters->View = View.ViewUniformBuffer;
	Parameters->InputTexture = SceneColor.Texture;
	Parameters->InputSampler = TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();
	Parameters->SceneTextures = Inputs.SceneTextures;
	FillParameters(*Parameters, Style, Split, View, SceneColor.Texture->Desc.Extent);
	Parameters->RenderTargets[0] = Output.GetRenderTargetBinding();

	const FGlobalShaderMap* ShaderMap = GetGlobalShaderMap(View.GetFeatureLevel());
	TShaderMapRef<TShaderClass> PixelShader(ShaderMap);

	AddDrawScreenPass(
		GraphBuilder,
		RDG_EVENT_NAME("DeusGoLook %s", PassName),
		View,
		FScreenPassTextureViewport(Output),
		FScreenPassTextureViewport(SceneColor),
		PixelShader,
		Parameters);

	return Output;
}

FScreenPassTexture FDeusGoLookViewExtension::BeforeUpscale_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessMaterialInputs& Inputs)
{
	return RunPass<FDeusGoLookDepthPS>(GraphBuilder, View, Inputs, RenderStyle, RenderSplit, TEXT("Depth"));
}

FScreenPassTexture FDeusGoLookViewExtension::AfterTonemap_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessMaterialInputs& Inputs)
{
	return RunPass<FDeusGoLookPS>(GraphBuilder, View, Inputs, RenderStyle, RenderSplit, TEXT("Grade"));
}