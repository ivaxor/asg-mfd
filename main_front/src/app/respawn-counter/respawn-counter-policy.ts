export interface RespawnCounterPolicy {
    priority: number,
    disabled: boolean,
    min?: number,
    max?: number,
    batch_size?: number,
}
