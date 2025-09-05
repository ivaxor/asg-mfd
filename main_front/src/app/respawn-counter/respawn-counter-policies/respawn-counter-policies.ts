import { Component, inject, Input, OnInit } from '@angular/core';
import { RespawnCounterInfo, RespawnCounterPolicy } from '../respawn-counter-info';
import { CommonModule } from '@angular/common';
import { Observable } from 'rxjs';
import { FormsModule } from '@angular/forms';
import { RespawnCounterService } from '../respawn-counter-service';

@Component({
  selector: 'app-respawn-counter-policies',
  imports: [CommonModule, FormsModule],
  templateUrl: './respawn-counter-policies.html',
  styleUrl: './respawn-counter-policies.scss'
})
export class RespawnCounterPolicies implements OnInit {
  respawnPolicyService = inject(RespawnCounterService);

  @Input()
  info$: Observable<RespawnCounterInfo> | undefined;
  info: RespawnCounterInfo | undefined = undefined;

  ngOnInit(): void {
    this.info$?.subscribe(info => this.info = info);
  }

  add(): void {
    this.info!.policies ??= [];

    const policy: RespawnCounterPolicy = {
      priority: this.info!.policies.length + 1,
      disabled: false,
      min: this.info!.policies.length + 1,
      max: this.info!.policies.length + 1,
      batch_size: this.info!.policies.length + 1,
    };
    this.info!.policies.push(policy);
    console.log(this.info!.policies);
  }

  save(): void {
    this.respawnPolicyService.set(this.info!).subscribe(() => { });
  }

  changePriority(policy: RespawnCounterPolicy, shift: number): void {
    const currentIndex = this.info!.policies.indexOf(policy);
    const newIndex = currentIndex + shift;
    const [movedPolicy] = this.info!.policies.splice(currentIndex, 1);
    this.info!.policies.splice(newIndex, 0, movedPolicy);

    this.info!.policies = this.info!.policies.map((p, i) => ({ ...p, priority: i + 1 }));
  }

  remove(policy: RespawnCounterPolicy): void {
    this.info!.policies = this.info!.policies
      .filter(p => p !== policy)
      .map((p, i) => ({ ...p, priority: i + 1 }));
    console.log(this.info!.policies);
  }
}
