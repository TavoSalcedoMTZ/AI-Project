#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UpgradeItem.generated.h"

class ULevelManager;

UCLASS(Blueprintable, BlueprintType)
class AIPROJECT_API UUpgradeItem : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shop|Settings")
	FText UpgradeName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Shop|Settings")
	int32 Cost = 100;

	UPROPERTY(BlueprintReadWrite, Category = "Shop|State")
	bool bIsPurchased = false;

	UPROPERTY(BlueprintReadWrite, Category = "Shop|State")
	int32 PurchaseCount = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shop|Requirements")
	TArray<TSubclassOf<UUpgradeItem>> RequiredUpgrades;


	UFUNCTION(BlueprintCallable, Category = "Shop|Logic")
	bool CanPurchase(ULevelManager* LevelManager, const TArray<UUpgradeItem*>& AllShopUpgrades);

	UFUNCTION(BlueprintNativeEvent, Category = "Shop|Logic")
	void OnUpgradePurchased();

public:
	virtual UWorld* GetWorld() const override;

	UFUNCTION(BlueprintCallable, Category = "Shop|Spawning")
	void SpawnItemForPlayer(TSubclassOf<AActor> ItemClassToSpawn);
};