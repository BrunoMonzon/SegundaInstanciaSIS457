// Fill out your copyright notice in the Description page of Project Settings.


#include "Board.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
ABoard::ABoard()
{
    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    // PIEZAS CREADAS
    NumPiecesT = CreateDefaultSubobject<UTextRenderComponent>(TEXT("NumPieces")); // se crea el default subobject que es el subobjeto
    NumPiecesT->SetRelativeRotation(FRotator(0, 180, 0)); // esto es para que el texto no salga al reves
    NumPiecesT->SetTextRenderColor(FColor::White);
    NumPiecesT->SetRelativeLocation(FVector(0, -190, 165));
    NumPiecesT->SetText(FText::FromString("Piezas Creadas: 1"));
    NumPiecesT->SetWorldSize(14); // tamano del texto


    // LINEAS COMPLETADAS
    CompletedLinesT = CreateDefaultSubobject<UTextRenderComponent>(TEXT("CompletedLines")); // se crea el default subobject que es el subobjeto
    CompletedLinesT->SetRelativeRotation(FRotator(0, 180, 0)); // esto es para que el texto no salga al reves
    CompletedLinesT->SetTextRenderColor(FColor::Green);
    CompletedLinesT->SetRelativeLocation(FVector(0, -190, 125));
    CompletedLinesT->SetText(FText::FromString("Lineas Completadas: 0"));
    CompletedLinesT->SetWorldSize(14); // tamano del texto



    // BLOQUES DESTROZADOS
    // DestroyedBlocksT es el componente de texto que se muestra en la pantalla de tipo UTextRenderComponent creado en el cpp
    //Los componentes de texto se crean dentro del constructor de Board
    DestroyedBlocksT = CreateDefaultSubobject<UTextRenderComponent>(TEXT("DestBlocks")); // se crea el default subobject que es el subobjeto
    DestroyedBlocksT->SetRelativeRotation(FRotator(0, 180, 0)); // esto es para que el texto no salga al reves 
    DestroyedBlocksT->SetTextRenderColor(FColor::Yellow);
    DestroyedBlocksT->SetRelativeLocation(FVector(0, -190, 95));
    DestroyedBlocksT->SetText(FText::FromString("Bloques Destruidos: 0"));
    DestroyedBlocksT->SetWorldSize(14); // tamano del texto

    // SCORE
    ScoreT = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Score")); // se crea el default subobject que es el subobjeto
    ScoreT->SetRelativeRotation(FRotator(0, 180, 0)); // esto es para que el texto no salga al reves
    ScoreT->SetTextRenderColor(FColor::Purple);
    ScoreT->SetRelativeLocation(FVector(0, -190, 105));
    ScoreT->SetText(FText::FromString("Puntaje: 0"));
    ScoreT->SetWorldSize(14); // tamano del texto

    RootComponent = DestroyedBlocksT;

    static ConstructorHelpers::FObjectFinder<USoundCue> LineRemove_Sound(TEXT("SoundCue'/Game/Sounds/line-remove_Cue.line-remove_Cue'"));
    if (LineRemove_Sound.Succeeded())
    {
        LineRemoveSoundCue = LineRemove_Sound.Object;
    }

    static ConstructorHelpers::FObjectFinder<USoundCue> MoveToEnd_Sound(TEXT("SoundCue'/Game/Sounds/force-hit_Cue.force-hit_Cue'"));
    if (MoveToEnd_Sound.Succeeded())
    {
        MoveToEndSoundCue = MoveToEnd_Sound.Object;
    }

    static ConstructorHelpers::FObjectFinder<USoundCue> GameStart_Sound(TEXT("SoundCue'/Game/Sounds/start_Cue.start_Cue'"));
    if (GameStart_Sound.Succeeded())
    {
        GameStartSoundCue = GameStart_Sound.Object;
    }

    static ConstructorHelpers::FObjectFinder<USoundCue> GameOver_Sound(TEXT("SoundCue'/Game/Sounds/gameover_Cue.gameover_Cue'"));
    if (GameOver_Sound.Succeeded())
    {
        GameOverSoundCue = GameOver_Sound.Object;
    }
}

// Called when the game starts or when spawned
void ABoard::BeginPlay()
{
    Super::BeginPlay();

    for (TActorIterator<APieces> it(GetWorld()); it; ++it)
    {
        if (it->GetFName() == TEXT("DissmissPieces"))
        {
            it->Dismiss();
            it->Destroy();
        }
    }
}

// Called every frame
void ABoard::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bGameOver)
    {
        return;
    }

    switch (Status)
    {
    case PS_NOT_INITED:
        Inicio();///////////////////////////////////////////////////Sellama a la funcion inicio
        CoolLeft = CoolDown;
        Status = PS_MOVING;
        break;
    case PS_MOVING:
        CoolLeft -= DeltaTime;
        if (CoolLeft <= 0.0f)
        {
            MoveDown();
        }
        break;
    case PS_GOT_BOTTOM:
        CoolLeft -= DeltaTime;
        if (CoolLeft <= 0.0f)
        {
            if (CurrentPieces)
            {
                CurrentPieces->Dismiss();
                CurrentPieces->Destroy();
            }
            CurrentPieces = nullptr;
            NewPieces();
            CoolLeft = CoolDown;
            Status = PS_MOVING;
        }
        break;
    default:
        break;
    }
}

// Called to bind functionality to input
void ABoard::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAction("Rotate", IE_Pressed, this, &ABoard::Rotate);
    PlayerInputComponent->BindAction("MoveLeft", IE_Pressed, this, &ABoard::MoveLeft);
    PlayerInputComponent->BindAction("MoveRight", IE_Pressed, this, &ABoard::MoveRight);
    PlayerInputComponent->BindAction("MoveDownToEnd", IE_Pressed, this, &ABoard::MoveDownToEnd);
    PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &ABoard::Pausa);
    //PlayerInputComponent->BindAction("NewPieces", IE_Pressed, this, &ABoard::NewPieces);
    //PlayerInputComponent->BindAction("CheckLine", IE_Pressed, this, &ABoard::CheckLine);
}

void ABoard::Rotate()
{
    if (CurrentPieces && Status != PS_GOT_BOTTOM)
    {
        CurrentPieces->TestRotate();
    }
}

void ABoard::MoveLeft()
{
    if (CurrentPieces)
    {
        CurrentPieces->MoveLeft();
        if (Status == PS_GOT_BOTTOM)
        {
            MoveDownToEnd();
        }
    }
}

void ABoard::MoveRight()
{
    if (CurrentPieces)
    {
        CurrentPieces->MoveRight();
        if (Status == PS_GOT_BOTTOM)
        {
            MoveDownToEnd();
        }
    }
}

void ABoard::MoveDown()
{
    if (CurrentPieces)
    {
        if (!CurrentPieces->MoveDown())
        {
            Status = PS_GOT_BOTTOM;
        }
        CoolLeft = CoolDown;
    }
}
/////////////////////////////////////////////////////////////////////////////

void ABoard::Inicio()///////////////////////////////////////////////funcion inicio
{
    FVector Location(0.0, 5.0, 195.0);//posicion de la primera pieza al inicio
    FRotator Rotation(0.0, 0.0, 0.0);//rotacion de la primera pieza al inicio
    CurrentPieces = GetWorld()->SpawnActor<APieces>(Location, Rotation);//se crea la primera pieza
    for (int i = 1; i <= NumeroP; i++)//ciclo for para crear las piezas que se van a usar
    {

        FVector Location(0.0, EspacionE_P, 195.0 - ((i - 1) * EntrePiezas));
        APieces* B = GetWorld()->SpawnActor<APieces>(Location, Rotation);
        Piezas.Add(B);
    }
}
void ABoard::Pausa()
{
    EliminacionTotal();
    Status = PS_NOT_INITED;
    for (APieces* B : Piezas)
    {
        B->Eliminar();
    }
    Piezas.Empty();
    CurrentPieces = nullptr;
    bGameOver = false;

}
void ABoard::NewPieces()
{
    CheckLine();
    if (CurrentPieces)
    {
        CurrentPieces->Dismiss();
        CurrentPieces->Destroy();
    }
    int i = 0;
    for (APieces* B : Piezas)
    {
        if (i == 0)
        {
            B->SetActorRelativeLocation(FVector(0.0, 5.0, 195.0));

            CurrentPieces = B;

        }
        else
        {
            B->SetActorRelativeLocation(FVector(0.0, EspacionE_P, 195.0 - ((i - 1) * EntrePiezas)));
        }
        i++;
    }
    Piezas.RemoveAt(0);
    FVector Location(0.0, 80.0, 195.0 - ((i - 1) * 40));
    FRotator Rotation(0.0, 0.0, 0.0);
    APieces* B = GetWorld()->SpawnActor<APieces>(Location, Rotation);
    PiecesNum += 1;
    NumPiecesT->SetText(FString::Printf(TEXT("Piezas Creadas: %d"), PiecesNum));

    Piezas.Add(B);
    bGameOver = CheckGameOver();
    if (bGameOver)
    {
        UE_LOG(LogTemp, Warning, TEXT("Game Over!!!!!!!!"));
        if (GameOverSoundCue)
        {
            UGameplayStatics::PlaySoundAtLocation(GetWorld(), GameOverSoundCue, GetActorLocation(), GetActorRotation());
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////////////

void ABoard::EliminacionTotal()
{
    auto MoveDownFromLine = [this](int z) {
        FVector Location(0.0f, 0.0f, 5.0 * z + 100.0);
        FRotator Rotation(0.0f, 0.0f, 0.0f);
        TArray<struct FOverlapResult> OutOverlaps;
        FCollisionShape CollisionShape;
        FVector Extent(4.5f, 49.5f, 95.0 + 4.5 - 5.0 * z);
        CollisionShape.SetBox(Extent);
        DrawDebugBox(GetWorld(), Location, Extent, FColor::Red, false, 1, 0, 1);//dibuja el area de colision
        FCollisionQueryParams Params;
        FCollisionResponseParams ResponseParam;
        if (GetWorld()->OverlapMultiByChannel(OutOverlaps,
            Location, Rotation.Quaternion(), ECollisionChannel::ECC_WorldDynamic,
            CollisionShape, Params, ResponseParam))
        {
            for (auto&& Result : OutOverlaps)
            {
                FVector NewLocation = Result.GetActor()->GetActorLocation();
                NewLocation.Z -= 10.0f;
                Result.GetActor()->SetActorLocation(NewLocation);
            }
        }
    };

    int z = 0;
    while (z < 40)/////////////////////////////////////////////////
    {
        FVector Location(0.0f, 0.0f, 10.0f * z + 5.0f);
        FRotator Rotation(0.0f, 0.0f, 0.0f);
        TArray<struct FOverlapResult> OutOverlaps;
        FCollisionShape CollisionShape;
        CollisionShape.SetBox(FVector(4.0f, 49.0f, 4.0f));
        //DrawDebugBox(GetWorld(), Location, FVector(4.5f, 49.5f, 4.5f), FColor::Purple, false, 1, 0, 1);
        FCollisionQueryParams Params;
        FCollisionResponseParams ResponseParam;
        bool b = GetWorld()->OverlapMultiByChannel(OutOverlaps,
            Location, Rotation.Quaternion(), ECollisionChannel::ECC_WorldDynamic,
            CollisionShape, Params, ResponseParam);
        if (!b || OutOverlaps.Num() < 1)//////////////////////////////////////////////////////
        {
            ++z;
            continue;
        }
        else // this line is full, remove the line
        {
            UE_LOG(LogTemp, Warning, TEXT("Find FULL LINE at z=%d"), z);
            for (auto&& Result : OutOverlaps)
            {
                Result.GetActor()->Destroy();
            }
            MoveDownFromLine(z);
        }
    }
    PiecesNum = 1;
    NumPiecesT->SetText(FString::Printf(TEXT("Piezas Creadas: %d"), PiecesNum));
    CompletedLinesNum = 0;
    CompletedLinesT->SetText(FString::Printf(TEXT("Lineas Completadas: %d"), CompletedLinesNum));
    DestroyedBlocksNum = 0;
    DestroyedBlocksT->SetText(FString::Printf(TEXT("Bloques Destruidos: %d"), DestroyedBlocksNum));
    ScoreNum = 0;
    ScoreT->SetText(FString::Printf(TEXT("Puntaje: %d"), ScoreNum));

}
void ABoard::CheckLine()
{
    auto MoveDownFromLine = [this](int z) {
        FVector Location(0.0f, 0.0f, 5.0 * z + 100.0);
        FRotator Rotation(0.0f, 0.0f, 0.0f);
        TArray<struct FOverlapResult> OutOverlaps;
        FCollisionShape CollisionShape;
        FVector Extent(4.5f, 49.5f, 95.0 + 4.5 - 5.0 * z);
        CollisionShape.SetBox(Extent);
        DrawDebugBox(GetWorld(), Location, Extent, FColor::Purple, false, 1, 0, 1);//dibuja el area de colision
        FCollisionQueryParams Params;
        FCollisionResponseParams ResponseParam;
        if (GetWorld()->OverlapMultiByChannel(OutOverlaps,
            Location, Rotation.Quaternion(), ECollisionChannel::ECC_WorldDynamic,
            CollisionShape, Params, ResponseParam))
        {
            for (auto&& Result : OutOverlaps)
            {
                FVector NewLocation = Result.GetActor()->GetActorLocation();
                NewLocation.Z -= 10.0f;
                Result.GetActor()->SetActorLocation(NewLocation);
            }
        }
    };

    int z = 0;
    while (z < 20)
    {
        FVector Location(0.0f, 0.0f, 10.0f * z + 5.0f);
        FRotator Rotation(0.0f, 0.0f, 0.0f);
        TArray<struct FOverlapResult> OutOverlaps;
        FCollisionShape CollisionShape;
        CollisionShape.SetBox(FVector(4.0f, 49.0f, 4.0f));
        FCollisionQueryParams Params;
        FCollisionResponseParams ResponseParam;
        bool b = GetWorld()->OverlapMultiByChannel(OutOverlaps,
            Location, Rotation.Quaternion(), ECollisionChannel::ECC_WorldDynamic,
            CollisionShape, Params, ResponseParam);
        if (!b || OutOverlaps.Num() < 10)
        {
            ++z;
            continue;
        }
        else // this line is full, remove the line
        {
            UE_LOG(LogTemp, Warning, TEXT("Find FULL LINE at z=%d"), z);
            for (auto&& Result : OutOverlaps)
            {
                Result.GetActor()->Destroy();
            }
            MoveDownFromLine(z);

            CompletedLinesNum += 1;
            CompletedLinesT->SetText(FString::Printf(TEXT("Lineas Completadas: %d"), CompletedLinesNum));

            DestroyedBlocksNum += 10;
            DestroyedBlocksT->SetText(FString::Printf(TEXT("Bloques Destruidos: %d"), DestroyedBlocksNum));

            ScoreNum += 100;
            ScoreT->SetText(FString::Printf(TEXT("Puntaje: %d"), ScoreNum));

            if (LineRemoveSoundCue)
            {
                UGameplayStatics::PlaySoundAtLocation(GetWorld(), LineRemoveSoundCue, GetActorLocation(), GetActorRotation());
            }
        }
    }
}

void ABoard::MoveDownToEnd()
{
    if (!CurrentPieces)
    {
        return;
    }

    while (CurrentPieces->MoveDown())
    {
    }

    if (MoveToEndSoundCue)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), MoveToEndSoundCue, GetActorLocation(), GetActorRotation());
    }

    switch (Status)
    {
    case PS_MOVING:
        Status = PS_GOT_BOTTOM;
        CoolLeft = CoolDown;
        break;
    case PS_GOT_BOTTOM:
        break;
    default:
        UE_LOG(LogTemp, Warning, TEXT("Wrong status for MoveDownToEnd"));
        break;
    }
}



bool ABoard::CheckGameOver()
{
    if (!CurrentPieces)
    {
        UE_LOG(LogTemp, Warning, TEXT("NoPieces"));
        return true;
    }

    return CurrentPieces->CheckWillCollision([](FVector OldVector) { return OldVector; });
}
