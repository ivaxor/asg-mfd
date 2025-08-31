import { inject, Injectable } from '@angular/core';
import { RespawnCounterPolicy } from './respawn-counter-policy';
import { map, Observable } from 'rxjs';
import { HttpClient } from '@angular/common/http';

@Injectable({
  providedIn: 'root',
})
export class RespawnCounterPolicyService {
  private http = inject(HttpClient);

  get(): Observable<RespawnCounterPolicy[]> {
    const url = `http://192.168.1.4/api/respawn-counter/policies`;
    return this.http.get<RespawnCounterPolicy[]>(url);
  }

  set(policies: RespawnCounterPolicy[]): Observable<void> {
    const url = `http://192.168.1.4/api/respawn-counter/policies`;
    return this.http.post<void>(url, policies);
  }
}
