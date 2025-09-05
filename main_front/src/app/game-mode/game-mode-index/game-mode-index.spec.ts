import { ComponentFixture, TestBed } from '@angular/core/testing';

import { GameModeIndex } from './game-mode-index';

describe('GameModeIndex', () => {
  let component: GameModeIndex;
  let fixture: ComponentFixture<GameModeIndex>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [GameModeIndex]
    })
    .compileComponents();

    fixture = TestBed.createComponent(GameModeIndex);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
