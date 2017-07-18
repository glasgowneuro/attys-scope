// Fill out your copyright notice in the Description page of Project Settings.

#include "MyAttysActor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Sets default values
AMyAttysActor::AMyAttysActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ListenSocket = NULL;

}


// Called when the game starts or when spawned
void AMyAttysActor::BeginPlay()
{
	Super::BeginPlay();

	StartUDPReceiver();

}


void AMyAttysActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
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
void AMyAttysActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	uint32 PendingData = 0;

	if (ListenSocket == nullptr) return;

	ListenSocket->HasPendingData(PendingData);

	if (PendingData > 0)
	{
		uint8 *attysdata = new uint8[PendingData];
		int32 BytesRead;
		ListenSocket->Recv(attysdata, PendingData, BytesRead);

		// AttysScope transmits first the raw data and then the filtered data
		// MAKE SURE THAT ALL 8 CHANNELS ARE ON AT ATTYS SCOPE SO THAT IT SENDS 8 FILTERED CHANNELS
		float t, accx, accy, accz, magx, magy, magz, adc0, adc1, filt0, filt1, filt2, filt3, filt4, filt5, filt6, filt7;

		// Scan it all in
		sscanf_s((char*)attysdata, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", &t, &accx, &accy, &accz, &magx, &magy, &magz, &adc0, &adc1,
			&filt0, &filt1, &filt2, &filt3, &filt4, &filt5, &filt6, &filt7);

		delete attysdata;

		FAttysCustomData data;

		data.filtered0 = filt0;
		data.filtered1 = filt1;
		data.filtered2 = filt2;
		data.filtered3 = filt3;
		data.filtered4 = filt4;
		data.filtered5 = filt5;
		data.filtered6 = filt6;
		data.filtered7 = filt7;

		//BP Event
		BPEvent_DataReceived(data);
	}

}


void AMyAttysActor::StartUDPReceiver() {

	ListenSocket = FUdpSocketBuilder(TEXT("AttysSocket"))
		.BoundToPort(65000)
		.Build();

	//UE_LOG(LogTemp, Warning, TEXT("UDP start"));
}
