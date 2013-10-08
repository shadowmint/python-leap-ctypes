#include <stdio.h>
#include <cleap.h>
#ifdef WIN32
#include <windows.h>
#endif
#ifndef WIN32
#include <unistd.h>
#endif

void sleep_(int ms) {
#ifdef WIN32
  Sleep(ms);
#else
  usleep(ms * 1000);
#endif
}

void print_frame(struct leap_frame *frame) {
  printf("Frame %ld: timestamp: %ld\n", frame->id, frame->timestamp);
}

void print_vector(struct leap_vector *v) {
  printf("%.2f %.2f %.2f", v->points[0], v->points[1], v->points[2]);
}

void print_bounds(struct leap_bounds *bounds) {
  printf("Interaction box: ");
  print_vector(&bounds->center);
  printf(" ");
  print_vector(&bounds->size);
  printf("\n");
}

void print_hand(struct leap_hand *hand) {
  printf("Hand: %d fingers. ", hand->finger_count);
  print_vector(&hand->palm_position);
  printf(" ");
  print_vector(&hand->palm_normal);
  printf(" ");
  print_vector(&hand->direction);
  printf(" Radius: %f\n", hand->sphere_radius);
}

void print_finger(struct leap_finger *finger) {
  printf("Finger: %d ", finger->id);
  print_vector(&finger->position);
  print_vector(&finger->velocity);
  print_vector(&finger->direction);
  printf("\n");
}

int main() {

  // setup
  struct leap_event *event;
  struct leap_controller *controller = leap_controller();
  struct leap_listener *listener = leap_listener(1000);
  leap_add_listener(controller, listener);

  // poll for events until we get 5000
  int limit = 5000;
  int counter = 0;
  int i, j;
  while (counter < limit) {
    while ((event = leap_poll_listener(listener)) != NULL) {
      printf("New event (%d): Type: %d\n", counter, event->event_code);
      print_frame(&event->frame);
      print_bounds(&event->frame.bounds);
      for (i = 0; i < event->frame.hand_count; i++) {
        struct leap_hand *hand = &event->frame.hands[i];
        print_hand(hand);
        for (j = 0; j < hand->finger_count; ++j) {
          struct leap_finger *finger = &hand->fingers[j];
          print_finger(finger);
        }
      }

      ++counter;
      if (counter > limit)
        break;
    }
    sleep_(100);
  }

  // shutdown
  leap_remove_listener(controller, listener);
  leap_listener_dispose(listener);
  leap_controller_dispose(controller);
}
