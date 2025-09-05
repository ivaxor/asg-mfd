import { Routes } from '@angular/router';
import { RespawnCounterIndex } from './respawn-counter/respawn-counter-index/respawn-counter-index';
import { GameModeIndex } from './game-mode/game-mode-index/game-mode-index';

export const routes: Routes = [
    { path: 'respawn-counter', component: RespawnCounterIndex, },
    { path: 'game-mode', component: GameModeIndex, },
];
