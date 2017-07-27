#include "AttysActor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Sets default values
AAttysActor::AAttysActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ListenSocket = NULL;

}


// Called when the game starts or when spawned
void AAttysActor::BeginPlay()
{
	Super::BeginPlay();

	StartUDPReceiver();

}


void AAttysActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	//Clear all sockets!
	//		makes sure repeat plays in Editor dont hold on to old sockets!
	if (ListenSocket)
	{
		ListenSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenSocket);
	}
	ListenSocket = nullptr;
}


// Called every frame
void AAttysActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (ListenSocket == nullptr) return;

	uint32 PendingData = 0;
	ListenSocket->HasPendingData(PendingData);

	if (PendingData > 0)
	{

		FAttysData data;
		uint8 attysdata[0x10000];
		int32 BytesRead;
		float t, accx, accy, accz, magx, magy, magz, adc0, adc1, filt0, filt1, filt2, filt3, filt4, filt5, filt6, filt7;

		// We save just the filtered ones
		data.filtered0 = 0;
		data.filtered1 = 0;
		data.filtered2 = 0;
		data.filtered3 = 0;
		data.filtered4 = 0;
		data.filtered5 = 0;
		data.filtered6 = 0;
		data.filtered7 = 0;

		int nSamples = 0;

		do {
			ListenSocket->Recv(attysdata, PendingData, BytesRead);
			nSamples++;

			// Scan it all in
			sscanf_s((char*)attysdata, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", &t, &accx, &accy, &accz, &magx, &magy, &magz, &adc0, &adc1,
				&filt0, &filt1, &filt2, &filt3, &filt4, &filt5, &filt6, &filt7);

			// We save just the filtered ones
			data.filtered0 += filt0;
			data.filtered1 += filt1;
			data.filtered2 += filt2;
			data.filtered3 += filt3;
			data.filtered4 += filt4;
			data.filtered5 += filt5;
			data.filtered6 += filt6;
			data.filtered7 += filt7;

			ListenSocket->HasPendingData(PendingData);

		} while (PendingData > 0);

		data.filtered0 /= nSamples;
		data.filtered1 /= nSamples;
		data.filtered2 /= nSamples;
		data.filtered3 /= nSamples;
		data.filtered4 /= nSamples;
		data.filtered5 /= nSamples;
		data.filtered6 /= nSamples;
		data.filtered7 /= nSamples;

		//BP Event
		BPEvent_AttysDataReceived(data);
	}
}


void AAttysActor::StartUDPReceiver() {

	ListenSocket = FUdpSocketBuilder(TEXT("AttysSocket"))
		.BoundToPort(65000)
		.Build();

	//UE_LOG(LogTemp, Warning, TEXT("UDP start"));
}
