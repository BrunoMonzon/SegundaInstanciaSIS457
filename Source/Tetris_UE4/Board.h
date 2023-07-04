// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Components/TextRenderComponent.h" // SE AGREGA LA LIBRERIA PARA MANEJAR COMPONENTES DE TEXTO TIPO OBJETO
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Pieces.h"
#include <functional>
#include "Board.generated.h"

UCLASS()
class TETRIS_UE4_API ABoard : public APawn
{
    GENERATED_BODY()

public:
    // Sets default values for this pawn's properties
    ABoard();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    void Rotate(); // 旋转90度，如果不能旋转则忽略
    void MoveLeft();  // 向左移动一格
    void MoveRight(); // 向右移动一格
    void MoveDown(); // 向下移动一格
    void NewPieces(); // 随机产生一个新的形状
    void CheckLine(); // 检查是否某一行可以消除了
    void EliminacionTotal();//agrgamos el metodo  
    void MoveDownToEnd(); // 向下移动到不能移动为止

    ///////////////////////
    void Inicio();
    void Pausa();
    //////////////////////
public:
    int modo = 0;
    UPROPERTY()
        APieces* CurrentPieces;

    UPROPERTY()
        class USoundCue* LineRemoveSoundCue;

    UPROPERTY()
        class USoundCue* MoveToEndSoundCue;

    UPROPERTY()
        class USoundCue* GameOverSoundCue;

    UPROPERTY()
        class USoundCue* GameStartSoundCue;
    ////////
   // Se agregan los componentes de texto de tipo UTextRenderComponent por cada dato 

    UTextRenderComponent* DestroyedBlocksT;
    UTextRenderComponent* CompletedLinesT;
    UTextRenderComponent* ScoreT;
    UTextRenderComponent* NumPiecesT;
    //////////

private:
    ////////////////////////////////////////
    // son variables int para almacenar la cantidad de:

    int32 DestroyedBlocksNum = 0; //Numero de bloques destruidos
    int32 CompletedLinesNum = 0; //Numero de lineas completadas
    int32 ScoreNum = 0; //Puntaje
    int32 PiecesNum = 1; //Numero de piezas creadas


    //Vector que contiene todas las piezas generadas
    TArray<APieces*> Piezas;//Vector que contiene todas las piezas generadas
    
    //variable que determina en numero de piezas que se van a mostrar
    const float NumeroP = 20.0f;//Numero de piezas

    const float EspacionE_P = 150.0f;//Espacio entre el ecenario y la pieza 

    const float EntrePiezas = 50.0f;//Espacio entre piezas
    /////////////////////////////
    enum PiecesStatus { PS_NOT_INITED, PS_MOVING, PS_GOT_BOTTOM };//Enum para determinar el estado de la pieza

    PiecesStatus Status = PS_NOT_INITED;//Variable que determina el estado de la pieza

    /////////////////////////////////////////
    float CoolDown = 0.5f;//Tiempo de espera para que la pieza baje
    float CoolLeft = 0.5f;//Tiempo restante para que la pieza baje
    bool bGameOver = false;//Variable que determina si el juego termino

    bool CheckGameOver();//Variable que determina si el juego termino
};
