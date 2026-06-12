#include "UpgradeItem.h"
#include "LevelManager/LevelManager.h"

bool UUpgradeItem::CanPurchase(ULevelManager* LevelManager, const TArray<UUpgradeItem*>& AllShopUpgrades)
{
	if (!LevelManager || bIsPurchased) return false;

	if (LevelManager->SharedMoney < Cost) return false;

	for (TSubclassOf<UUpgradeItem> RequiredClass : RequiredUpgrades)
	{
		if (!RequiredClass) continue;

		bool bRequirementMet = false;

		for (UUpgradeItem* ItemInShop : AllShopUpgrades)
		{
			if (ItemInShop && ItemInShop->IsA(RequiredClass))
			{
				if (ItemInShop->bIsPurchased)
				{
					bRequirementMet = true;
					break;
				}
			}
		}

		if (!bRequirementMet) return false;
	}

	return true;
}

void UUpgradeItem::OnUpgradePurchased_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Mejora %s procesada en C++"), *UpgradeName.ToString());
}