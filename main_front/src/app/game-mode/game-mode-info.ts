import { Time } from "@angular/common";

export enum GameMode {
    RespawnCounter,
    Bomb,
    Flag,
}

export interface GameModeInfo {
    mode: GameMode,
    start_timestamp: number,
    timestamp: number,
}

export interface GameModeInfoExtended extends GameModeInfo {
    start_date: Date,
    duration: number,
}