#include "UpgradeItem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "LevelManager/LevelManager.h"

bool UUpgradeItem::CanPurchase(ULevelManager* LevelManager, const TArray<UUpgradeItem*>& AllShopUpgrades)
{
	if (!LevelManager) return false;

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

UWorld* UUpgradeItem::GetWorld() const
{
	// Si no es un objeto por defecto y tiene un "creador" (Outer), tomamos el mundo de su creador
	if (!HasAnyFlags(RF_ClassDefaultObject) && GetOuter())
	{
		return GetOuter()->GetWorld();
	}
	return nullptr;
}

void UUpgradeItem::SpawnItemForPlayer(TSubclassOf<AActor> ItemClassToSpawn)
{
	if (!ItemClassToSpawn) return;

	UWorld* World = GetWorld();
	if (!World) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	if (PC)
	{
		APawn* PlayerPawn = PC->GetPawn();
		if (PlayerPawn)
		{
			FVector SpawnLocation = PlayerPawn->GetActorLocation() + FVector(200.f, 0.f, 150.f);
			FRotator SpawnRotation = FRotator::ZeroRotator;

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			World->SpawnActor<AActor>(ItemClassToSpawn, SpawnLocation, SpawnRotation, SpawnParams);

			UE_LOG(LogTemp, Warning, TEXT("Objeto spawneado exitosamente sobre el jugador."));
		}
	}
}