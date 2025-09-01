import { ComponentFixture, TestBed } from '@angular/core/testing';

import { SettingsIndex } from './settings-index';

describe('SettingsIndex', () => {
  let component: SettingsIndex;
  let fixture: ComponentFixture<SettingsIndex>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [SettingsIndex]
    })
    .compileComponents();

    fixture = TestBed.createComponent(SettingsIndex);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
