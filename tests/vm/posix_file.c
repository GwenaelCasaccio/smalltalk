#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include <cmocka.h>

#include "libgst/gstpriv.h"

#include "libgst/sysdep.h"

#include "libgst/sysdep/posix/files.c"

pid_t __wrap_fork() {
  function_called();

  return (pid_t) mock();
}

int __wrap_pipe(int pipefd[2]) {
  static int args = 123;

  function_called();

  pipefd[0] = args++;
  pipefd[1] = args++;

  return (int) mock();
}

int __wrap_fcntl(int fd, int cmd, ...) {
  function_called();

  return (int)mock();
}

SigHandler _gst_set_signal_handler(int signum, SigHandler handlerFunc) {
  return NULL;
}

void __wrap_perror(const char *s) {}

void __wrap_nomemory(bool b) {}

static void should_execute_ls_command(void **state) {

  (void)state;

  char *const argv[] = { NULL };
  OOP os_process_oop = malloc(sizeof(*os_process_oop));
  gst_object os_process = calloc(100, sizeof(*os_process));
  OOP_SET_OBJECT(os_process_oop, os_process);
  OOP stdin_oop = malloc(sizeof(*stdin_oop));
  gst_object stdin_obj = calloc(100, sizeof(*stdin_obj));
  OOP_SET_OBJECT(stdin_oop, stdin_obj);
  OOP stdout_oop = malloc(sizeof(*stdout_oop));
  gst_object stdout_obj = calloc(100, sizeof(*stdout_obj));
  OOP_SET_OBJECT(stdout_oop, stdout_obj);
  OOP stderr_oop = malloc(sizeof(*stderr_oop));
  gst_object stderr_obj = calloc(100, sizeof(*stderr_obj));
  OOP_SET_OBJECT(stderr_oop, stderr_obj);

  OBJ_OS_PROCESS_SET_STDIN(os_process, stdin_oop);
  OBJ_OS_PROCESS_SET_STDOUT(os_process, stdout_oop);
  OBJ_OS_PROCESS_SET_STDERR(os_process, stderr_oop);

  expect_function_calls(__wrap_pipe, 1);
  will_return(__wrap_pipe, 0);
  expect_function_calls(__wrap_pipe, 1);
  will_return(__wrap_pipe, 0);
  expect_function_calls(__wrap_fcntl, 2);
  will_return(__wrap_fcntl, 0);
  will_return(__wrap_fcntl, 0);
  expect_function_calls(__wrap_pipe, 1);
  will_return(__wrap_pipe, 0);
  expect_function_calls(__wrap_fcntl, 2);
  will_return(__wrap_fcntl, 0);
  will_return(__wrap_fcntl, 0);

  expect_function_calls(__wrap_fork, 1);
  will_return(__wrap_fork, 123);

  int res = _gst_exec_command_with_fd("ls", argv, -2, -2, -2, os_process_oop);

  assert_int_equal(res, 0);
  assert_int_equal(TO_INT(OBJ_OS_PROCESS_GET_PID(os_process)), 123);
  assert_int_equal(TO_INT(OBJ_FILE_STREAM_GET_FD(
                       OOP_TO_OBJ(OBJ_OS_PROCESS_GET_STDIN(os_process)))),
                   124);
  assert_int_equal(TO_INT(OBJ_FILE_STREAM_GET_FD(
                       OOP_TO_OBJ(OBJ_OS_PROCESS_GET_STDOUT(os_process)))),
                   125);
  assert_int_equal(TO_INT(OBJ_FILE_STREAM_GET_FD(
                       OOP_TO_OBJ(OBJ_OS_PROCESS_GET_STDERR(os_process)))),
                   127);
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(should_execute_ls_command)
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
