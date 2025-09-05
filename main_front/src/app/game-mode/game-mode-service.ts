import { HttpClient } from '@angular/common/http';
import { inject, Injectable } from '@angular/core';
import { SettingsService } from '../settings/settings-service';
import { GameMode, GameModeInfo } from './game-mode-info';
import { Observable } from 'rxjs';

@Injectable({
  providedIn: 'root'
})
export class GameModeService {
  private httpClient = inject(HttpClient);
  private settingsService = inject(SettingsService);

  get(): Observable<GameModeInfo> {
    const settings = this.settingsService.get();
    const url = `${settings.base_url}/api/game-mode`;
    return this.httpClient.get<GameModeInfo>(url);
  }

  set(mode: GameMode): Observable<void> {
    const settings = this.settingsService.get();
    const url = `${settings.base_url}/api/game-mode`;
    return this.httpClient.post<void>(url, { mode });
  }
}
