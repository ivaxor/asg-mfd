import { ComponentFixture, TestBed } from '@angular/core/testing';

import { RespawnCounterIndex } from './respawn-counter-index';

describe('RespawnCounterIndex', () => {
  let component: RespawnCounterIndex;
  let fixture: ComponentFixture<RespawnCounterIndex>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [RespawnCounterIndex]
    })
    .compileComponents();

    fixture = TestBed.createComponent(RespawnCounterIndex);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
