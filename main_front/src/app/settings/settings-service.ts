import { Injectable } from '@angular/core';
import { Settings } from './settings';

@Injectable({
  providedIn: 'root'
})
export class SettingsService {
  private localStorageKey: string = "settings";

  get(): Settings {
    const json = localStorage.getItem(this.localStorageKey)
    if (json === null)
      return this.setToDefault();

    const settings: Settings = JSON.parse(json);
    return settings;
  }

  setToDefault(): Settings {
    const settings: Settings = {
      base_url: "http://192.168.4.1",
    };
    const json = JSON.stringify(settings);
    localStorage.setItem(this.localStorageKey, json);
    return settings;
  }
}
