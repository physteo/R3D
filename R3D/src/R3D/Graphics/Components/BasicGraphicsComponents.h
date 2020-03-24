#pragma once

#include <R3D/Core/MathUtils.h>

namespace r3d
{

	struct SegmentPrimitive {};

	struct CirclePrimitive {};

	struct SquarePrimitive {};

	struct BoxPrimitive {};

	struct SpherePrimitive {};

	struct PlanePrimitive {};

	struct Color
	{
		Color(const float4& vector = float4{ 1.0f, 0.0f, 0.0f, 1.0f }) : vec(vector) {}	
		float4 vec;
	};

#if 0
	struct ShaderComponent
	{
		ShaderComponent() : openglId(-1) {}
		ShaderComponent(size_t openglIdIn) : openglId(openglIdIn) {}
		size_t openglId;
	};

	struct TextureComponent
	{
		TextureComponent() : openglId(0), internalId(0) {}
		TextureComponent(std::pair<size_t, size_t> idxPair)
			: openglId(idxPair.first), internalId(idxPair.second) {}

		TextureComponent(size_t openglIdIn, size_t internalIdIn)
			: openglId(openglIdIn), internalId(internalIdIn) {}
		size_t openglId;
		size_t internalId;
	};

	struct TextureTile
	{
		TextureTile() : tile_x(0), tile_y(0), mirror_x(0.0f) {}
		TextureTile(short tile_xIn, short tile_yIn, float mirror_xIn) : tile_x(tile_xIn), tile_y(tile_yIn), mirror_x(mirror_xIn) {}
		short tile_x;
		short tile_y;
		float mirror_x;
	};

	struct AnimationFrame
	{
		AnimationFrame() : frameDeltat(0.01f), elapsed(0.0f), //totalElapsed(0.0f),
			currentStep(0), currentAnimationId(0),
			defaultAnimationId(0), numberOfFrames(1),
			reversed(false), repeated(true) {}
		AnimationFrame(float frameDeltatIn, float elapsedIn, //float totalElapsedIn,
			size_t currentStepIn, size_t currentAnimationIdIn,
			size_t defaultAnimationIdIn, size_t numberOfFramesIn,
			bool reversedIn, bool repeatedIn)
			: frameDeltat(frameDeltatIn), elapsed(elapsedIn), //totalElapsed(totalElapsedIn),
			currentStep(currentStepIn), currentAnimationId(currentAnimationIdIn), defaultAnimationId(defaultAnimationIdIn),
			numberOfFrames(numberOfFramesIn), reversed(reversedIn), repeated(repeatedIn) {}

		float frameDeltat;
		float elapsed;
		//float totalElapsed;

		size_t currentStep;
		size_t currentAnimationId;
		size_t defaultAnimationId;
		size_t numberOfFrames;

		bool reversed;
		bool repeated;
	};
#endif

}