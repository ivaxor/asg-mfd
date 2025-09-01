import { Component, inject, OnInit } from '@angular/core';
import { RespawnCounterInfo } from '../respawn-counter-info';
import { RespawnCounterService } from '../respawn-counter-service';
import { CommonModule } from '@angular/common';

@Component({
  selector: 'app-respawn-counter-policies',
  imports: [CommonModule],
  templateUrl: './respawn-counter-policies.html',
  styleUrl: './respawn-counter-policies.scss'
})
export class RespawnCounterPolicies implements OnInit {
  private respawnCounterService = inject(RespawnCounterService);

  info: RespawnCounterInfo | undefined = undefined;  

  ngOnInit(): void {
    this.respawnCounterService.get().subscribe(i => this.info = i);
  }
}
