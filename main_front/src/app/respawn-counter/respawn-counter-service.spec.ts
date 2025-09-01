import { TestBed } from '@angular/core/testing';

import { RespawnCounterService } from '../respawn-counter-service';

describe('RespawnCounterService', () => {
  let service: RespawnCounterService;

  beforeEach(() => {
    TestBed.configureTestingModule({});
    service = TestBed.inject(RespawnCounterService);
  });

  it('should be created', () => {
    expect(service).toBeTruthy();
  });
});
