import { Component, inject } from '@angular/core';
import { RespawnCounterPolicies } from '../respawn-counter-policies/respawn-counter-policies';
import { RespawnCounterInfo } from '../respawn-counter-info';
import { RespawnCounterService } from '../respawn-counter-service';
import { Observable } from 'rxjs';
import { CommonModule } from '@angular/common';

@Component({
  selector: 'app-respawn-counter-index',
  imports: [CommonModule, RespawnCounterPolicies],
  templateUrl: './respawn-counter-index.html',
  styleUrl: './respawn-counter-index.scss'
})
export class RespawnCounterIndex {
  private respawnCounterService = inject(RespawnCounterService);

  info$: Observable<RespawnCounterInfo> | undefined;

  ngOnInit(): void {
    this.info$ = this.respawnCounterService.get();
  }
}
