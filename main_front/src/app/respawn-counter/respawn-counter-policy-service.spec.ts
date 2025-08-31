import { TestBed } from '@angular/core/testing';

import { RespawnCounterPolicyService } from './respawn-counter-policy-service';

describe('RespawnCounterPolicyService', () => {
  let service: RespawnCounterPolicyService;

  beforeEach(() => {
    TestBed.configureTestingModule({});
    service = TestBed.inject(RespawnCounterPolicyService);
  });

  it('should be created', () => {
    expect(service).toBeTruthy();
  });
});
