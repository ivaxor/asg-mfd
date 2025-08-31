import { Component, signal } from '@angular/core';
import { RouterOutlet } from '@angular/router';
import { AppNav } from './app-nav/app-nav';

@Component({
  selector: 'app-root',
  imports: [RouterOutlet, AppNav],
  templateUrl: './app.html',
  styleUrl: './app.scss'
})
export class App {
  protected readonly title = signal('ASG-MFD');
}
