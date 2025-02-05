/*
  Copyright (C) 2011, 2012 by Roger E Critchlow Jr, Santa Fe, NM, USA.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
*/
#ifndef KEYED_TONE_H
#define KEYED_TONE_H
/*
** keyed tone
** generates a tone with specified frequency and gain
**
** There are several possible keying envelopes
** sine uses 1/4 of a sine wave: sin(0) .. sin(pi/2); or sin(pi/2) .. sin(pi)
** raised cosine uses 1/2 of a sine wave: 1+cos(-pi) .. 1+cos(0); or 1+cos(0) .. 1+cos(pi)
** blackman-harris uses halves of the blackman-harris window function
** basically, all of the window functions are possibilities, but once you think about it
** you go with blackman-harris because it minimises key clicks.
**
** one might argue to use the window function itself as the entire keying envelope,
** stretching it to cover the entire dit or dah, rather than separating the attack,
** sustain, and decay phases and stitching them together.  this would mean that you
** know the length of the pulse when you start it, the rise and fall would be 1/2 the
** pulse length, and the keyed envelope would be smooth.
**
** but you don't usually know the pulse length until key up.
*/

#include "dspmath.h"
#include "oscillator.h"
#include "ramp.h"

#define KEYED_TONE_OFF	0	/* note is not sounding */
#define KEYED_TONE_RISE	1	/* note is ramping up to full level */
#define KEYED_TONE_ON	2	/* note is sounding full level */
#define KEYED_TONE_FALL	3	/* note is ramping down to off */
#define KEYED_TONE_RISE_THEN_FALL 4 /* note is ramping up, but off received */
#define KEYED_TONE_FALL_THEN_RISE 5 /* note is ramping down, but on received */

typedef struct {
  int state;			/* state of cwtone */
  float gain;			/* target gain */
  oscillator_t tone;		/* tone oscillator */
  ramp_t rise;			/* tone on ramp */
  ramp_t fall;			/* tone off ramp */
} keyed_tone_t;

static void keyed_tone_update(keyed_tone_t *p, float gain_dB, float freq, float rise, float fall, int window, int window2, unsigned sample_rate) {
  p->gain = powf(10.0f, gain_dB / 20.0f);
  oscillator_update(&p->tone, freq, sample_rate);
  ramp_update(&p->rise, 1, rise, window, window2, sample_rate);
  ramp_update(&p->fall, 0, fall, window, window2, sample_rate);
}

static void *keyed_tone_init(keyed_tone_t *p, float gain_dB, float freq, float rise, float fall, int window, int window2, unsigned sample_rate) {
  p->state = KEYED_TONE_OFF;
  p->gain = powf(10.0f, gain_dB / 20.0f);
  oscillator_init(&p->tone, freq, 0.0f, sample_rate);
  ramp_init(&p->rise, 1, rise, window, window2, sample_rate);
  ramp_init(&p->fall, 0, fall, window, window2, sample_rate);
  return p;
}

static void keyed_tone_on(keyed_tone_t *p) {
  switch (p->state) {
  case KEYED_TONE_OFF:
    p->state = KEYED_TONE_RISE;
    oscillator_set_zero_phase(&p->tone);
    ramp_start(&p->rise);
    break;			/* start rise */
  case KEYED_TONE_RISE:
  case KEYED_TONE_ON:
    break;			/* ignore repeated on */
  case KEYED_TONE_FALL:
    p->state = KEYED_TONE_FALL_THEN_RISE;
    break;			/* defer rise to end of fall */
  case KEYED_TONE_RISE_THEN_FALL:
    p->state = KEYED_TONE_RISE;
    break;			/* cancel momentary off */
  case KEYED_TONE_FALL_THEN_RISE:
    break;			/* ignore repeated on */
  }
}

static void keyed_tone_off(keyed_tone_t *p) {
  switch (p->state) {
  case KEYED_TONE_OFF:
    break;			/* ignore repeated off */
  case KEYED_TONE_RISE:
    p->state = KEYED_TONE_RISE_THEN_FALL;
    break;			/* defer fall to end of rise */
  case KEYED_TONE_ON:
    p->state = KEYED_TONE_FALL;
    ramp_start(&p->fall);
    break;
  case KEYED_TONE_FALL:
  case KEYED_TONE_RISE_THEN_FALL:
    break;			/* ignore repeated off */
  case KEYED_TONE_FALL_THEN_RISE:
    p->state = KEYED_TONE_FALL;
    break;			/* cancel momentary on */
  }
}

static float _Complex keyed_tone_process(keyed_tone_t *p) {
  float scale = p->gain;
  // printf("scale: %f, state: %d\n", scale, p->state);
  switch (p->state) {
  case KEYED_TONE_OFF:	/* note is not sounding */
    scale = 0;
    break;
  case KEYED_TONE_RISE:	/* note is ramping up to full level */
  case KEYED_TONE_RISE_THEN_FALL:
    scale *= ramp_next(&p->rise);
    if (ramp_done(&p->rise))
      if (p->state == KEYED_TONE_RISE_THEN_FALL) {
	p->state = KEYED_TONE_FALL;
	ramp_start(&p->fall);
      } else
	p->state = KEYED_TONE_ON;
    break;
  case KEYED_TONE_ON:	/* note is sounding full level */
    break;
  case KEYED_TONE_FALL:	/* note is ramping down to off */
  case KEYED_TONE_FALL_THEN_RISE:
    scale *= ramp_next(&p->fall);
    if (ramp_done(&p->fall))
      if (p->state == KEYED_TONE_FALL_THEN_RISE) {
	p->state = KEYED_TONE_RISE;
	ramp_start(&p->rise);
      } else
	p->state = KEYED_TONE_OFF;
    break;
  }
  double opv = oscillator_process(&p->tone);
  // printf("scale: %f, opv: %f\n", scale, opv);
  return scale * opv;
}
#endif
