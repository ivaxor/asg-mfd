export interface RespawnCounterInfo {
    current_respawn_tokens: number;
    respawn_tokens: number;
    policies: RespawnCounterPolicy[];
    policies_length: number;
}

export interface RespawnCounterPolicy {
    priority: number,
    disabled: boolean,
    min: number,
    max: number,
    batch_size: number,
}