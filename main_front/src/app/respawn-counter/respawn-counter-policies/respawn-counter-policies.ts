import { Component, inject, OnInit } from '@angular/core';
import { RespawnCounterPolicy } from '../respawn-counter-policy';
import { RespawnCounterPolicyService } from '../respawn-counter-policy-service';
import { CommonModule } from '@angular/common';

@Component({
  selector: 'app-respawn-counter-policies',
  imports: [CommonModule],
  templateUrl: './respawn-counter-policies.html',
  styleUrl: './respawn-counter-policies.scss'
})
export class RespawnCounterPolicies implements OnInit {
  private service = inject(RespawnCounterPolicyService);

  policies: RespawnCounterPolicy[] | undefined = undefined;  

  ngOnInit(): void {
    this.service.get().subscribe(p => this.policies = p);
  }
}
