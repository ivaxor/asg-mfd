import { inject, Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { HttpClient } from '@angular/common/http';
import { SettingsService } from '../settings/settings-service';
import { RespawnCounterInfo } from './respawn-counter-info';

@Injectable({
  providedIn: 'root'
})
export class RespawnCounterService {
  private httpClient = inject(HttpClient);
  private settingsService = inject(SettingsService);

  get(): Observable<RespawnCounterInfo> {
    const settings = this.settingsService.get();
    const url = `${settings.base_url}/api/respawn-counter`;
    return this.httpClient.get<RespawnCounterInfo>(url);
  }

  set(info: RespawnCounterInfo): Observable<void> {
    const settings = this.settingsService.get();
    const url = `${settings.base_url}/api/respawn-counter`;
    info.policies = info.policies.sort(p => p.priority);
    return this.httpClient.post<void>(url, info);
  }
}
