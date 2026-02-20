export enum GameMode {
    RespawnCounter,    
    Flag,
    Bomb,
}

export interface GameModeInfo {
    voltage: number,
    current: number,
    mode: GameMode,
    start_timestamp: number,
    timestamp: number,
}

export interface GameModeInfoExtended extends GameModeInfo {
    start_date: Date,
    duration: number,
}