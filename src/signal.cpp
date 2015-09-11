/*

***********************************************************
* © Benoit Leveau, inc. All rights reserved. *
***********************************************************

*/

#include <termios.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <iostream>
#include <signal.h>

#include "signal.hpp"

int getch(void)
{
      int c=0;

      struct termios org_opts, new_opts;
      int res=0;
          //-----  store old settings -----------
      res=tcgetattr(STDIN_FILENO, &org_opts);
      assert(res==0);
          //---- set new terminal parms --------
      memcpy(&new_opts, &org_opts, sizeof(new_opts));
      new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
      tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
      c=getchar();
          //------  restore old settings ---------
      res=tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
      assert(res==0);
      return(c);
}

void signal_handler(int arg)
{
    if (SIGINT == arg)
    {
        while (1)
        {
            std::cout << "\nDo you want to stop the execution? [y/n] ";
            char c = getch();
            if (c == 'y')
            {
                std::cout << "\nInterrupted by user!\n";
                ::exit(EXIT_SUCCESS);
            }
            else if (c == 'n')
            {
                std::cout << std::endl;
                // restore signal handler
                install_handler();
                break;
            }
        }
    }
}

void install_handler_block()
{    
    struct sigaction setup_action;
    sigset_t block_mask;

    sigemptyset (&block_mask);
    /* Block other terminal-generated signals while handler runs. */
    sigaddset (&block_mask, SIGINT);
    sigaddset (&block_mask, SIGQUIT);
    sigaddset (&block_mask, SIGTSTP);
    setup_action.sa_handler = signal_handler;
    setup_action.sa_mask = block_mask;
    setup_action.sa_flags = 0;
    sigaction (SIGINT, &setup_action, NULL);
}

void install_handler_simple()
{
    ::signal(SIGINT, signal_handler);
}

void install_handler()
{
    install_handler_simple();
    // install_handler_block();
}
