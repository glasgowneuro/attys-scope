#pragma once



#include "CoreMinimal.h"

#include "GameFramework/Actor.h"

#include "Networking.h"

#include "AttysActor.generated.h"







USTRUCT(BlueprintType)

struct FAttysData

{

	GENERATED_USTRUCT_BODY()



		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joy Color")

		float filtered0 = 0.f;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joy Color")

		float filtered1 = 0.f;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joy Color")

		float filtered2 = 0.f;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joy Color")

		float filtered3 = 0.f;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joy Color")

		float filtered4 = 0.f;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joy Color")

		float filtered5 = 0.f;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joy Color")

		float filtered6 = 0.f;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joy Color")

		float filtered7 = 0.f;



	FAttysData()

	{}

};





UCLASS()

class SCROLLER_API AAttysActor : public AActor

{

	GENERATED_BODY()



public:

	// Sets default values for this actor's properties

	AAttysActor();



protected:

	// Called when the game starts or when spawned

	virtual void BeginPlay() override;



public:

	/** Called whenever this actor is being removed from a level */

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;



public:


	virtual void Tick(float DeltaTime) override;

	FSocket* ListenSocket = nullptr;

	void StartUDPReceiver();

	UFUNCTION(BlueprintImplementableEvent)
		void BPEvent_AttysDataReceived(const FAttysData& ReceivedData);

};
