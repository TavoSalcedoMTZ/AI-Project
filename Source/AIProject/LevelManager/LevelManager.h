#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LevelManager.generated.h"

// Equivalente a [Serializable] en Unity
USTRUCT(BlueprintType)
struct FRoundData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Round Settings")
	int32 TotalEnemies = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Round Settings")
	float SpawnRate = 2.0f;
};

/**
 * Level Manager para controlar Rondas y Enemigos
 */
UCLASS()
class AIPROJECT_API ULevelManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// Inicia el sistema de la ronda actual
	UFUNCTION(BlueprintCallable, Category = "Level Manager")
	void StartRound(int32 RoundNumber);

	// Se llama cuando un enemigo muere para dar dinero
	UFUNCTION(BlueprintCallable, Category = "Level Manager")
	void OnEnemyKilled(int32 PlayerID, int32 MoneyReward);

	// Función que intentará spawnear un enemigo según el ratio
	UFUNCTION()
	void SpawnEnemyRoutine();

	// Configuración de rondas (Puedes llenarlo desde Blueprints)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Manager|Data")
	TArray<FRoundData> RoundsConfig;

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:
	int32 CurrentRoundIndex = 0;
	int32 EnemiesSpawnedThisRound = 0;
	int32 EnemiesAlive = 0;

	// Variables para rastrear el dinero de los jugadores (0 = Jugador 1, 1 = Jugador 2)
	int32 Player1Money = 0;
	int32 Player2Money = 0;

	FTimerHandle SpawnTimerHandle;
};