// Shared helper for the placeholder-swap pattern used by every character class
// in the project (AWDPlayerCharacter, AWDSecurityAgent, AWDGovernmentNPC):
// each ships a visible primitive-mesh stand-in body (PlaceholderBodyMesh) with
// the real USkeletalMeshComponent hidden. The moment a content author assigns
// a real Skeletal Mesh (a MetaHuman, a Marketplace character) to that
// component - typically in a Blueprint child, no C++ change required - this
// flips the visibility automatically so the placeholder never has to be
// manually toggled off. See Docs/VISUAL_FIDELITY_ROADMAP.md.
#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"

namespace WDCharacterVisuals
{
	inline void ApplyPlaceholderSwap(USkeletalMeshComponent* RealMesh, UStaticMeshComponent* Placeholder)
	{
		const bool bHasRealMesh = RealMesh && RealMesh->GetSkeletalMeshAsset() != nullptr;

		if (RealMesh)
		{
			RealMesh->SetVisibility(bHasRealMesh, true);
		}
		if (Placeholder)
		{
			Placeholder->SetVisibility(!bHasRealMesh);
		}
	}
}
