#pragma once

#include "CoreMinimal.h"
#include "DeusGoLookTypes.generated.h"

// property names match the old flat config keys, see UDeusGoLookSettings::PostInitProperties

USTRUCT(BlueprintType, meta = (GroupHelp = "Distance wash that fades far geometry into a flat color"))
struct DEUSGOLOOK_API FDeusGoLookFog
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fog", meta = (DisplayName = "Enable Fog", ToolTip = "Distance wash, the biggest part of the look"))
	bool bFog = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fog", meta = (EditCondition = "bFog", DisplayName = "Color", HideAlphaChannel))
	FLinearColor FogColor = FLinearColor(0.08f, 0.10f, 0.13f, 1.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fog", meta = (EditCondition = "bFog", DisplayName = "Brightness", ClampMin = "0", UIMax = "8", ToolTip = "Multiplies the color, lower if the fog glows"))
	float FogIntensity = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fog", meta = (EditCondition = "bFog", DisplayName = "Start Distance", ClampMin = "0", Units = "cm", ToolTip = "Everything closer stays clear"))
	float FogStart = 2500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fog", meta = (EditCondition = "bFog", DisplayName = "Fade Length", ClampMin = "1", Units = "cm", ToolTip = "Distance from Start Distance to thickest fog"))
	float FogRange = 6000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fog", meta = (EditCondition = "bFog", DisplayName = "Max Opacity", ClampMin = "0", ClampMax = "1", ToolTip = "How much the thickest fog hides"))
	float FogMaxOpacity = 0.7f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fog", meta = (EditCondition = "bFog", DisplayName = "Falloff", ClampMin = "0.25", ClampMax = "4", ToolTip = "Above 1 keeps the foreground clear then thickens fast"))
	float FogCurve = 1.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fog", meta = (EditCondition = "bFog", DisplayName = "Drain Color", ClampMin = "0", ClampMax = "1", ToolTip = "Greys things out as they go into the fog"))
	float DistanceDesaturation = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fog", meta = (DisplayName = "Fog Noise", ToolTip = "Slow drifting patches so fog and mist are not a perfect gradient"))
	bool bFogNoise = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fog", meta = (EditCondition = "bFogNoise", DisplayName = "Noise Amount", ClampMin = "0", ClampMax = "1"))
	float FogNoiseAmount = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fog", meta = (EditCondition = "bFogNoise", DisplayName = "Noise Size", ClampMin = "100", Units = "cm", ToolTip = "Size of one patch in the world"))
	float FogNoiseScale = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fog", meta = (EditCondition = "bFogNoise", DisplayName = "Noise Drift", ClampMin = "0", UIMax = "500", ToolTip = "Wind speed in cm per second"))
	float FogNoiseSpeed = 50.f;
};

USTRUCT(BlueprintType, meta = (GroupHelp = "Replaces the sky and empty space with a flat color or a gradient"))
struct DEUSGOLOOK_API FDeusGoLookBackground
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Background", meta = (DisplayName = "Replace Sky", ToolTip = "Paints empty space a flat color"))
	bool bBackground = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Background", meta = (EditCondition = "bBackground", DisplayName = "Color", HideAlphaChannel))
	FLinearColor BackgroundColor = FLinearColor(0.05f, 0.07f, 0.09f, 1.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Background", meta = (EditCondition = "bBackground", DisplayName = "Brightness", ClampMin = "0", UIMax = "8"))
	float BackgroundIntensity = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Background", meta = (EditCondition = "bBackground", DisplayName = "Sky Distance", ClampMin = "1000", Units = "cm", ToolTip = "Anything farther counts as sky"))
	float BackgroundDistance = 50000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Background", meta = (EditCondition = "bBackground", DisplayName = "Sky Gradient", ToolTip = "Fades from Color at the bottom of the screen to Top Color at the top"))
	bool bSkyGradient = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Background", meta = (EditCondition = "bBackground && bSkyGradient", DisplayName = "Top Color", HideAlphaChannel))
	FLinearColor SkyTopColor = FLinearColor(0.02f, 0.03f, 0.04f, 1.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Background", meta = (EditCondition = "bBackground && bSkyGradient", DisplayName = "Gradient Falloff", ClampMin = "0.25", ClampMax = "4", ToolTip = "Above 1 keeps the bottom color over more of the screen"))
	float SkyGradientCurve = 1.2f;
};

USTRUCT(BlueprintType, meta = (GroupHelp = "Low fog by world height, fills the ground and makes the level float"))
struct DEUSGOLOOK_API FDeusGoLookMist
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mist", meta = (DisplayName = "Enable Ground Mist", ToolTip = "Low lying mist by world height, makes the level look like it floats"))
	bool bMist = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mist", meta = (EditCondition = "bMist", DisplayName = "Color", HideAlphaChannel))
	FLinearColor MistColor = FLinearColor(0.80f, 0.86f, 0.92f, 1.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mist", meta = (EditCondition = "bMist", DisplayName = "Brightness", ClampMin = "0", UIMax = "8"))
	float MistIntensity = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mist", meta = (EditCondition = "bMist", DisplayName = "Top Height", Units = "cm", ToolTip = "World height where the mist ends, thickest below"))
	float MistHeight = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mist", meta = (EditCondition = "bMist", DisplayName = "Fade Height", ClampMin = "1", Units = "cm", ToolTip = "How far below Top Height it reaches full thickness"))
	float MistFalloff = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mist", meta = (EditCondition = "bMist", DisplayName = "Max Opacity", ClampMin = "0", ClampMax = "1"))
	float MistOpacity = 0.6f;
};

USTRUCT(BlueprintType, meta = (GroupHelp = "Color grade over the whole image, applied after tonemapping"))
struct DEUSGOLOOK_API FDeusGoLookGrade
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grade", meta = (ClampMin = "0", ClampMax = "1", ToolTip = "0 full color, 1 black and white"))
	float Desaturation = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grade", meta = (HideAlphaChannel, ToolTip = "Tint for dark areas"))
	FLinearColor ShadowTint = FLinearColor(0.80f, 0.88f, 0.95f, 1.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grade", meta = (HideAlphaChannel, ToolTip = "Tint for bright areas"))
	FLinearColor HighlightTint = FLinearColor(1.0f, 0.93f, 0.78f, 1.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grade", meta = (ClampMin = "0.5", ClampMax = "2", ToolTip = "1 leaves it untouched"))
	float Contrast = 1.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grade", meta = (ClampMin = "0", ClampMax = "1", ToolTip = "Darkens the screen corners"))
	float Vignette = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grade", meta = (DisplayName = "Accent Hue", ToolTip = "Keeps one hue fully saturated while Desaturation greys out everything else"))
	bool bAccent = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grade", meta = (EditCondition = "bAccent", DisplayName = "Accent Color", HideAlphaChannel, ToolTip = "Only the hue is used, brightness does not matter"))
	FLinearColor AccentColor = FLinearColor(1.0f, 0.68f, 0.12f, 1.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grade", meta = (EditCondition = "bAccent", DisplayName = "Accent Range", ClampMin = "0.01", ClampMax = "0.5", ToolTip = "How far around the hue counts, 0.08 is just the gold family, 0.5 is everything"))
	float AccentRange = 0.08f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grade", meta = (EditCondition = "bAccent", DisplayName = "Accent Strength", ClampMin = "0", ClampMax = "1", ToolTip = "1 escapes the desaturation completely"))
	float AccentStrength = 1.f;
};

USTRUCT(BlueprintType, meta = (GroupHelp = "Ink lines from depth, outlines around shapes and shading in inside corners"))
struct DEUSGOLOOK_API FDeusGoLookEdges
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edges", meta = (DisplayName = "Enable Outlines", ToolTip = "Lines around object silhouettes"))
	bool bOutlines = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edges", meta = (EditCondition = "bOutlines", DisplayName = "Color From Surface", ToolTip = "Ink each outline with a darker version of the object's own color instead of Outline Color"))
	bool bOutlineFromSurface = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edges", meta = (EditCondition = "bOutlines && bOutlineFromSurface", DisplayName = "Surface Darkness", ClampMin = "0", ClampMax = "1", ToolTip = "How much darker than the surface the line is"))
	float OutlineSurfaceDarkness = 0.55f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edges", meta = (EditCondition = "bOutlines && !bOutlineFromSurface", HideAlphaChannel))
	FLinearColor OutlineColor = FLinearColor(0.15f, 0.18f, 0.22f, 1.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edges", meta = (EditCondition = "bOutlines", ClampMin = "0", ClampMax = "1"))
	float OutlineStrength = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edges", meta = (EditCondition = "bOutlines", DisplayName = "Outline Sensitivity", ClampMin = "0.0002", ClampMax = "0.05", ToolTip = "Lower catches more edges, too low gets noisy"))
	float OutlineDepthThreshold = 0.0025f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edges", meta = (DisplayName = "Enable Creases", ToolTip = "Darkens inside corners, reads as contact shadow"))
	bool bCreases = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edges", meta = (EditCondition = "bCreases", HideAlphaChannel))
	FLinearColor CreaseColor = FLinearColor(0.35f, 0.40f, 0.46f, 1.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edges", meta = (EditCondition = "bCreases", ClampMin = "0", ClampMax = "1"))
	float CreaseStrength = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edges", meta = (EditCondition = "bCreases", DisplayName = "Crease Sensitivity", ClampMin = "0.02", ClampMax = "1", ToolTip = "Lower catches shallower corners"))
	float CreaseNormalThreshold = 0.20f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edges", meta = (EditCondition = "bOutlines || bCreases", DisplayName = "Line Width", ClampMin = "0.5", ClampMax = "4", ToolTip = "In pixels"))
	float OutlineThickness = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edges", meta = (EditCondition = "bOutlines || bCreases", DisplayName = "Softness", ClampMin = "0.05", ClampMax = "2", ToolTip = "Higher gives softer, steadier lines"))
	float EdgeSoftness = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edges", meta = (EditCondition = "bOutlines || bCreases", DisplayName = "Fade Distance", ClampMin = "0", Units = "cm", ToolTip = "Lines fade out by this distance, 0 never fades"))
	float EdgeFadeDistance = 6000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Edges", meta = (EditCondition = "bOutlines || bCreases", DisplayName = "Far Width", ClampMin = "0.25", ClampMax = "1", ToolTip = "Line width reached at Fade Distance, as a fraction of Line Width. 1 keeps lines the same width everywhere"))
	float OutlineFarWidth = 0.5f;
};

UENUM(BlueprintType)
enum class EDeusGoLookPattern : uint8
{
	Triangles,
	Hexagons
};

// one stencil value, 5 to 8
USTRUCT(BlueprintType)
struct DEUSGOLOOK_API FDeusGoLookPatternSlot
{
	GENERATED_BODY()

	FDeusGoLookPatternSlot() = default;
	FDeusGoLookPatternSlot(EDeusGoLookPattern InShape, float InSize)
		: Shape(InShape)
		, Size(InSize)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patterns")
	EDeusGoLookPattern Shape = EDeusGoLookPattern::Triangles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patterns", meta = (ClampMin = "5", UIMax = "400", Units = "cm", ToolTip = "Width of one panel in the world"))
	float Size = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patterns", meta = (HideAlphaChannel, ToolTip = "Multiplies the whole surface, white leaves its color alone"))
	FLinearColor Tint = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patterns", meta = (DisplayName = "Facet Variation", ClampMin = "0", ClampMax = "1", ToolTip = "Brightness difference between panels, reads as tilted facets"))
	float FacetVariation = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patterns", meta = (DisplayName = "Groove Width", ClampMin = "0.02", ClampMax = "0.4", ToolTip = "Fraction of the panel"))
	float GrooveWidth = 0.08f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patterns", meta = (DisplayName = "Groove Darkness", ClampMin = "0", ClampMax = "1"))
	float GrooveDarkness = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patterns", meta = (DisplayName = "Groove Mesh Edges", ToolTip = "Adds a groove along the mesh border and its corners, so panels end on a seam instead of being cut off"))
	bool bEdgeGrooves = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patterns", meta = (DisplayName = "Bevel Color", HideAlphaChannel, ToolTip = "Lit edge along one side of each groove"))
	FLinearColor BevelColor = FLinearColor(1.0f, 0.78f, 0.35f, 1.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patterns", meta = (DisplayName = "Bevel Strength", ClampMin = "0", UIMax = "4"))
	float BevelStrength = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patterns", meta = (DisplayName = "Motion Direction", ClampMin = "-180", ClampMax = "180", Units = "Degrees", ToolTip = "Direction for Scroll and Wave, 0 is world +X on floors"))
	float MotionAngle = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patterns", meta = (DisplayName = "Scroll Speed", ClampMin = "0", UIMax = "500", Units = "cm/s", ToolTip = "Slides the whole pattern, 0 keeps it still"))
	float ScrollSpeed = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patterns", meta = (DisplayName = "Shimmer", ClampMin = "0", ClampMax = "1", ToolTip = "Panels flicker in brightness, each on its own rhythm, 0 is off"))
	float ShimmerAmount = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patterns", meta = (DisplayName = "Shimmer Speed", EditCondition = "ShimmerAmount > 0", ClampMin = "0", UIMax = "8", ToolTip = "Flickers per second"))
	float ShimmerSpeed = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patterns", meta = (DisplayName = "Wave", ClampMin = "0", UIMax = "4", ToolTip = "A band of panels lighting up in Bevel Color, sweeping along Motion Direction, 0 is off"))
	float WaveStrength = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patterns", meta = (DisplayName = "Wave Speed", EditCondition = "WaveStrength > 0", ClampMin = "0", UIMax = "20", ToolTip = "Panels per second"))
	float WaveSpeed = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patterns", meta = (DisplayName = "Wave Spacing", EditCondition = "WaveStrength > 0", ClampMin = "2", UIMax = "64", ToolTip = "Panels between two bands"))
	float WaveSpacing = 12.f;
};

USTRUCT(BlueprintType, meta = (GroupHelp = "Panel patterns on chosen meshes. On a mesh tick Render CustomDepth Pass and set CustomDepth Stencil Value 5 to 8 to pick a slot"))
struct DEUSGOLOOK_API FDeusGoLookPatterns
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patterns", meta = (DisplayName = "Enable Surface Patterns", ToolTip = "Needs Project Settings > Rendering > Custom Depth-Stencil Pass set to Enabled with Stencil"))
	bool bPatterns = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patterns", meta = (EditCondition = "bPatterns", DisplayName = "Stencil 5"))
	FDeusGoLookPatternSlot Slot5 = FDeusGoLookPatternSlot(EDeusGoLookPattern::Triangles, 60.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patterns", meta = (EditCondition = "bPatterns", DisplayName = "Stencil 6"))
	FDeusGoLookPatternSlot Slot6 = FDeusGoLookPatternSlot(EDeusGoLookPattern::Hexagons, 60.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patterns", meta = (EditCondition = "bPatterns", DisplayName = "Stencil 7"))
	FDeusGoLookPatternSlot Slot7 = FDeusGoLookPatternSlot(EDeusGoLookPattern::Triangles, 150.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patterns", meta = (EditCondition = "bPatterns", DisplayName = "Stencil 8"))
	FDeusGoLookPatternSlot Slot8 = FDeusGoLookPatternSlot(EDeusGoLookPattern::Hexagons, 25.f);
};

UENUM(BlueprintType)
enum class EDeusGoLookNeonLine : uint8
{
	Solid,
	Dashed,
	MarchingAnts UMETA(DisplayName = "Marching Ants")
};

// one stencil value, 1 to 4
USTRUCT(BlueprintType)
struct DEUSGOLOOK_API FDeusGoLookNeonZone
{
	GENERATED_BODY()

	FDeusGoLookNeonZone() = default;
	FDeusGoLookNeonZone(const FLinearColor& InColor, EDeusGoLookNeonLine InLineStyle, float InPulseAmount, float InPulseSpeed)
		: Color(InColor)
		, LineStyle(InLineStyle)
		, PulseAmount(InPulseAmount)
		, PulseSpeed(InPulseSpeed)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Neon", meta = (HideAlphaChannel))
	FLinearColor Color = FLinearColor(0.0f, 0.9f, 1.0f, 1.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Neon", meta = (ClampMin = "0", UIMax = "4", ToolTip = "Multiplies the master Glow for this zone only"))
	float Brightness = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Neon", meta = (DisplayName = "Line Style"))
	EDeusGoLookNeonLine LineStyle = EDeusGoLookNeonLine::Solid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Neon", meta = (DisplayName = "Pulse", ClampMin = "0", ClampMax = "1", ToolTip = "How much the glow breathes, 0 is steady"))
	float PulseAmount = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Neon", meta = (ClampMin = "0", UIMax = "6", ToolTip = "Pulses per second, fast reads as danger"))
	float PulseSpeed = 0.5f;
};

USTRUCT(BlueprintType, meta = (GroupHelp = "Glowing zones. On a mesh tick Render CustomDepth Pass and set CustomDepth Stencil Value 1 to 4 to pick a zone"))
struct DEUSGOLOOK_API FDeusGoLookNeon
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Neon", meta = (DisplayName = "Enable Neon Zones", ToolTip = "On a mesh tick Render CustomDepth Pass and set CustomDepth Stencil Value 1 to 4 to pick a color. Needs Project Settings > Rendering > Custom Depth-Stencil Pass set to Enabled with Stencil"))
	bool bNeon = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Neon", meta = (EditCondition = "bNeon", DisplayName = "Stencil 1"))
	FDeusGoLookNeonZone Zone1 = FDeusGoLookNeonZone(FLinearColor(1.0f, 0.68f, 0.12f, 1.f), EDeusGoLookNeonLine::Solid, 0.15f, 0.4f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Neon", meta = (EditCondition = "bNeon", DisplayName = "Stencil 2"))
	FDeusGoLookNeonZone Zone2 = FDeusGoLookNeonZone(FLinearColor(0.1f, 0.8f, 1.0f, 1.f), EDeusGoLookNeonLine::MarchingAnts, 0.15f, 0.4f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Neon", meta = (EditCondition = "bNeon", DisplayName = "Stencil 3"))
	FDeusGoLookNeonZone Zone3 = FDeusGoLookNeonZone(FLinearColor(1.0f, 0.12f, 0.08f, 1.f), EDeusGoLookNeonLine::Solid, 0.6f, 2.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Neon", meta = (EditCondition = "bNeon", DisplayName = "Stencil 4", ToolTip = "Stencil 5 to 8 are Surface Patterns, not neon"))
	FDeusGoLookNeonZone Zone4 = FDeusGoLookNeonZone(FLinearColor(0.25f, 1.0f, 0.3f, 1.f), EDeusGoLookNeonLine::Solid, 0.15f, 0.4f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Neon", meta = (EditCondition = "bNeon", DisplayName = "Glow", ClampMin = "0", UIMax = "30", ToolTip = "Master brightness for every zone, raise until bloom picks it up and it starts to halo"))
	float NeonGlow = 4.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Neon", meta = (EditCondition = "bNeon", DisplayName = "Line Width", ClampMin = "1", ClampMax = "8", ToolTip = "In pixels"))
	float NeonWidth = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Neon", meta = (EditCondition = "bNeon", DisplayName = "Fill", ClampMin = "0", ClampMax = "1", ToolTip = "Glow across the whole surface, 0 keeps just the outline"))
	float NeonFill = 0.08f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Neon", meta = (EditCondition = "bNeon", DisplayName = "Hex Fill", ToolTip = "Glowing hexagon grid on the zone's surface, adds to Fill"))
	bool bNeonHex = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Neon", meta = (EditCondition = "bNeon && bNeonHex", DisplayName = "Hex Size", ClampMin = "5", UIMax = "200", Units = "cm", ToolTip = "Width of one hexagon in the world"))
	float NeonHexSize = 40.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Neon", meta = (EditCondition = "bNeon && bNeonHex", DisplayName = "Hex Strength", ClampMin = "0", ClampMax = "1"))
	float NeonHexStrength = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Neon", meta = (EditCondition = "bNeon && bNeonHex", DisplayName = "Hex Line Width", ClampMin = "0.02", ClampMax = "0.3", ToolTip = "Fraction of a hexagon"))
	float NeonHexLine = 0.08f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Neon", meta = (EditCondition = "bNeon", DisplayName = "Dash Length", ClampMin = "2", UIMax = "40", ToolTip = "For Dashed and Marching Ants zones, in pixels"))
	float NeonDashLength = 8.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Neon", meta = (EditCondition = "bNeon", DisplayName = "Ants Speed", ClampMin = "0", UIMax = "200", ToolTip = "How fast Marching Ants dashes crawl, pixels per second"))
	float NeonDashSpeed = 24.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Neon", meta = (EditCondition = "bNeon", DisplayName = "Show Through Walls", ToolTip = "Dim outline where something hides the zone"))
	bool bNeonShowOccluded = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Neon", meta = (EditCondition = "bNeon && bNeonShowOccluded", DisplayName = "Through Walls Opacity", ClampMin = "0", ClampMax = "1"))
	float NeonOccludedOpacity = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Neon", meta = (EditCondition = "bNeon", DisplayName = "Ignore Fog", ToolTip = "Zones stay clear of fog and mist so they pop"))
	bool bNeonIgnoreFog = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Neon", meta = (EditCondition = "bNeon", DisplayName = "Keep Saturated", ToolTip = "Pre boosts the colors so the Grade Desaturation does not dull them"))
	bool bNeonKeepSaturated = true;
};

USTRUCT(BlueprintType, meta = (GroupHelp = "Screen effects on top of the grade, each one can be switched on alone"))
struct DEUSGOLOOK_API FDeusGoLookStylize
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stylize", meta = (DisplayName = "Enable Posterize", ToolTip = "Snaps colors to a few flat bands, applied on top of the grade"))
	bool bPosterize = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stylize", meta = (EditCondition = "bPosterize", DisplayName = "Bands", ClampMin = "2", ClampMax = "32", ToolTip = "Color steps per channel, lower is flatter"))
	int32 PosterizeLevels = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stylize", meta = (EditCondition = "bPosterize", DisplayName = "Posterize Strength", ClampMin = "0", ClampMax = "1"))
	float PosterizeStrength = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stylize", meta = (DisplayName = "Enable Tilt Shift", ToolTip = "Blurs the top and bottom of the screen for a miniature look"))
	bool bTiltShift = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stylize", meta = (EditCondition = "bTiltShift", DisplayName = "Focus Position", ClampMin = "0", ClampMax = "1", ToolTip = "0 top of the screen, 1 bottom"))
	float TiltShiftFocus = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stylize", meta = (EditCondition = "bTiltShift", DisplayName = "Focus Size", ClampMin = "0", ClampMax = "1", ToolTip = "Height of the sharp band, fraction of the screen"))
	float TiltShiftWidth = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stylize", meta = (EditCondition = "bTiltShift", DisplayName = "Blur Amount", ClampMin = "0", ClampMax = "24", ToolTip = "Blur radius at the screen edges, in pixels"))
	float TiltShiftBlur = 6.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stylize", meta = (DisplayName = "Enable Grain", ToolTip = "Fine moving noise, hides banding in fog gradients"))
	bool bGrain = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stylize", meta = (EditCondition = "bGrain", DisplayName = "Grain Amount", ClampMin = "0", ClampMax = "0.15"))
	float GrainAmount = 0.02f;
};

USTRUCT(BlueprintType)
struct DEUSGOLOOK_API FDeusGoLookStyle
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Look")
	FDeusGoLookFog Fog;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Look")
	FDeusGoLookBackground Background;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Look")
	FDeusGoLookGrade Grade;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Look")
	FDeusGoLookEdges Edges;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Look", meta = (DisplayName = "Ground Mist"))
	FDeusGoLookMist Mist;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Look", meta = (DisplayName = "Neon Zones"))
	FDeusGoLookNeon Neon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Look", meta = (DisplayName = "Surface Patterns"))
	FDeusGoLookPatterns Patterns;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Look")
	FDeusGoLookStylize Stylize;
};
