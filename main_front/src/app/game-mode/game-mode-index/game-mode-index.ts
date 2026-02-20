import { Component, inject } from '@angular/core';
import { GameModeService } from '../game-mode-service';
import { GameMode, GameModeInfoExtended } from '../game-mode-info';
import { CommonModule } from '@angular/common';
import { map, Observable, switchMap, timer } from 'rxjs';

@Component({
  selector: 'app-game-mode-index',
  imports: [CommonModule],
  templateUrl: './game-mode-index.html',
  styleUrl: './game-mode-index.scss'
})
export class GameModeIndex {
  private gameModeService = inject(GameModeService);

  gameMode = GameMode;
  info$: Observable<GameModeInfoExtended> | undefined;

  ngOnInit(): void {
    this.info$ = timer(0, 5000).pipe(
      switchMap(() => this.gameModeService.get()),
      map(info => {
        const duration = (info.timestamp - info.start_timestamp) / 1000;

        return {
          voltage: info.voltage,
          current: info.current,
          mode: info.mode,
          start_timestamp: info.start_timestamp,
          timestamp: info.timestamp,
          start_date: new Date(Date.now() - duration),
          duration: duration,
        };
      }));
  }
}
