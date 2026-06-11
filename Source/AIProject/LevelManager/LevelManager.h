#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LevelManager.generated.h"

class UUpgradeItem;
class UUserWidget; // Forward declaration para no sobrecargar el header

USTRUCT(BlueprintType)
struct FRoundData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Round Settings")
	int32 TotalEnemies = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Round Settings")
	float SpawnRate = 2.0f;
};

UCLASS()
class AIPROJECT_API ULevelManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Level Manager|Economy")
	int32 SharedMoney = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Manager|Shop")
	TArray<TSubclassOf<UUpgradeItem>> ShopUpgradesClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Manager|Enemies")
	TSubclassOf<AActor> NormalEnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Manager|Enemies")
	TSubclassOf<AActor> SpecialEnemyClass;

	// === NUEVO: Variable para asignar tu Widget BPW_Shop ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Manager|UI")
	TSubclassOf<UUserWidget> ShopWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "Level Manager|Shop")
	TArray<UUpgradeItem*> InstancedShopUpgrades;

	UFUNCTION(BlueprintCallable, Category = "Level Manager|Shop")
	void InitializeShop();

	UFUNCTION(BlueprintCallable, Category = "Level Manager|Shop")
	bool TryPurchaseUpgrade(UUpgradeItem* UpgradeToBuy);

	UFUNCTION(BlueprintCallable, Category = "Level Manager")
	void StartRound(int32 RoundNumber);

	UFUNCTION(BlueprintCallable, Category = "Level Manager")
	void OnEnemyKilled(int32 PlayerID, int32 MoneyReward);

	UFUNCTION()
	void SpawnEnemyRoutine();

	// === NUEVAS FUNCIONES PARA CONTROL DE TIENDA ENTRE RONDAS ===
	UFUNCTION(BlueprintCallable, Category = "Level Manager")
	void OnRoundFinalized();

	UFUNCTION()
	void AdvanceToNextRound();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Manager|Data")
	TArray<FRoundData> RoundsConfig;

	// === NUEVO: Control de área de Spawn ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Manager|Spawn")
	FVector SpawnCenter = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Manager|Spawn")
	float SpawnRadius = 2000.0f;

	// Función para llamar desde el Blueprint del Nivel al iniciar BeginPlay
	UFUNCTION(BlueprintCallable, Category = "Level Manager|Spawn")
	void SetSpawnArea(FVector NewCenter, float NewRadius);

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:
	int32 CurrentRoundIndex = 0;
	int32 EnemiesSpawnedThisRound = 0;
	int32 EnemiesAlive = 0;

	FTimerHandle SpawnTimerHandle;

	// === NUEVAS VARIABLES PRIVADAS ===
	FTimerHandle ShopPhaseTimerHandle;

	UPROPERTY()
	UUserWidget* CurrentShopWidget = nullptr; // Guarda el widget vivo para poder quitarlo después
};