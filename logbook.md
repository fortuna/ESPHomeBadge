# Log Book

In this document I track some thoughts and observations from my development sessions.


## Log

### May 25, 2025 - Page Selector

Open questions:
- Does focus stay on one page, or goes across pages? It switched across pages.

I decided to use pages for navigation. My expectation is that pages will fix the cross-screen focus issue.

It turns out focus will switch across pages, so it doesn't fix the problem.

ESPHome lets you nest styles (it compiles), but the nested style seems to be ignored. Perhaps we should use C++ instead.

I moved the LVGL code to its own `lvgl.yaml` file that I can now include into both the badge firmware and the host prototyping code.

TODO:
- Fix focus. Consider not using the system focus for the page selector.

### May 25, 2025 - App Switcher

Open questions:
- How to make the app switcher zoom out and scroll properly?
- Why does everything disappear when the transformed widget has a transparent background?

Working on making a cool App Switcher work.

I want to use the Button 1 (Back/ESC) to make the screen zoom out into a strip that shows all
available screens and you can navigate to switch between them.

I want to use native scrolling to minizime code I need to write and maintain. It also help others
extend.

I found that styles can have transition effects, so I will probably not need to implement my own ([example][Transition Example]).

A scroll callback may be helpful, as used in the [translate on scroll example].

I'm using size transformation to scale the screens, but that breaks positioning and scroll. I found out I need to specify an anchor point for the transformation, but that didn't fix all the issues. After a lot of time debugging, I finally found out that the widget layout size is not affected by the transformation (see [Positions, sizes, and layouts]).

Plan is to create empty containers as proxies for the scaled screens, then anchor the scaled screens to those containers.

[Transition Example]: https://docs.lvgl.io/8.2/overview/style.html#transition
[translate on scroll example]: https://docs.lvgl.io/8/overview/scroll.html#translate-on-scroll
[Positions, sizes, and layouts]: https://docs.lvgl.io/9.2/overview/coord.html#transformation


### Previously - Focus issues

The tabbed navigation did not work well, because the focus moves across tabs, while I need it to stay within a tab. I also didn't like how it looks.

I tried switching to the tileview. It had the same issue. But it gave me the idea of an app switcher where
you can zoom in and out of.
