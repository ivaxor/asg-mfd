import { ComponentFixture, TestBed } from '@angular/core/testing';

import { RespawnCounterPolicies } from './respawn-counter-policies';

describe('RespawnCounterPolicies', () => {
  let component: RespawnCounterPolicies;
  let fixture: ComponentFixture<RespawnCounterPolicies>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [RespawnCounterPolicies]
    })
    .compileComponents();

    fixture = TestBed.createComponent(RespawnCounterPolicies);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
