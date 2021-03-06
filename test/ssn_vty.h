
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <vty_server.h>


/*
 *======================================
 */

/* echo */
vty_cmd_match echo_mt()
{
  vty_cmd_match m;
  m.nodes.push_back(new node_fixedstring("echo", ""));
  m.nodes.push_back(new node_string              );
  return m;
}
void echo_f(vty_cmd_match* m, vty_client* sh, void* arg)
{
  std::string s = m->nodes[1]->get();
  sh->Printf("%s\n", s.c_str());
}

/* show_author */
vty_cmd_match show_author_mt()
{
  vty_cmd_match m;
  m.nodes.push_back(new node_fixedstring("show", ""));
  m.nodes.push_back(new node_fixedstring("author", ""));
  return m;
}
void show_author_f(vty_cmd_match* m, vty_client* sh, void* arg)
{
  sh->Printf("Hiroki SHIROKURA.\r\n");
  sh->Printf(" Twitter : @\r\n");
  sh->Printf(" Github  : \r\n");
  sh->Printf(" Facebook: hiroki.shirokura\r\n");
  sh->Printf(" E-mail  : slank.dev@gmail.com\r\n");
}


/* show_version */
vty_cmd_match show_version_mt()
{
  vty_cmd_match m;
  m.nodes.push_back(new node_fixedstring("show", ""));
  m.nodes.push_back(new node_fixedstring("version", ""));
  return m;
}
void show_version_f(vty_cmd_match* m, vty_client* sh, void* arg)
{
  sh->Printf("Susanow 0.0.0\r\n");
  sh->Printf("Copyright 2017-2020 Hiroki SHIROKURA.\r\n");
}

/* quit */
vty_cmd_match quit_mt()
{
  vty_cmd_match m;
  m.nodes.push_back(new node_fixedstring("quit", ""));
  return m;
}
void quit_f(vty_cmd_match* m, vty_client* sh, void* arg)
{
  sh->close();
}

/* clear */
vty_cmd_match clear_mt()
{
  vty_cmd_match m;
  m.nodes.push_back(new node_fixedstring("clear", ""));
  return m;
}
void clear_f(vty_cmd_match* m, vty_client* sh, void* arg)
{
  sh->Printf("\033[2J\r\n");
}

/* list */
vty_cmd_match list_mt()
{
  vty_cmd_match m;
  m.nodes.push_back(new node_fixedstring("list", ""));
  return m;
}
void list_f(vty_cmd_match* m, vty_client* sh, void* arg)
{
  const std::vector<vty_cmd*>& commands = *sh->commands;
  for (vty_cmd* cmd : commands) {
    std::string s = "";
    for (node* nd : cmd->match.nodes) {
      s += nd->to_string();
      s += " ";
    }
    sh->Printf("  %s\r\n", s.c_str());
  }
}

/* slank */
vty_cmd_match slank_mt()
{
  vty_cmd_match m;
  m.nodes.push_back(new node_fixedstring("slank", ""));
  return m;
}
void slank_f(vty_cmd_match* m, vty_client* sh, void* arg)
{
  sh->Printf("slankdev \r\n");
}


/*
 *======================================
 */

class ssn_vty {
  vty_server* v;
 public:
  ssn_vty(uint32_t addr, uint16_t port)
  {
    char str[] = "\r\n"
        "Hello, this is Susanow (version 0.00.00.0).\r\n"
        "Copyright 2017-2020 Hiroki SHIROKURA.\r\n"
        "\r\n"
        " .d8888b.                                                             \r\n"
        "d88P  Y88b                                                            \r\n"
        "Y88b.                                                                 \r\n"
        " \"Y888b.   888  888 .d8888b   8888b.  88888b.   .d88b.  888  888  888 \r\n"
        "    \"Y88b. 888  888 88K          \"88b 888 \"88b d88\"\"88b 888  888  888 \r\n"
        "      \"888 888  888 \"Y8888b. .d888888 888  888 888  888 888  888  888 \r\n"
        "Y88b  d88P Y88b 888      X88 888  888 888  888 Y88..88P Y88b 888 d88P \r\n"
        " \"Y8888P\"   \"Y88888  88888P\' \"Y888888 888  888  \"Y88P\"   \"Y8888888P\"  \r\n"
        "\r\n";
    v = new vty_server(addr, port, str, "ssn> ");
    v->install_command(slank_mt       (), slank_f       , nullptr);
    v->install_command(quit_mt        (), quit_f        , nullptr);
    v->install_command(clear_mt       (), clear_f       , nullptr);
    v->install_command(echo_mt        (), echo_f        , nullptr);
    v->install_command(list_mt        (), list_f        , nullptr);
    v->install_command(show_author_mt (), show_author_f , nullptr);
    v->install_command(show_version_mt(), show_version_f, nullptr);
  }
  virtual ~ssn_vty() { delete v; }

  void poll_dispatch() { v->poll_dispatch(); }
  void install_command(vty_cmd_match m, vty_cmdcallback_t f, void* arg)
  { v->install_command(m, f, arg); }
};

bool vty_running;
void vty_poll(void* arg)
{
  ssn_vty* v = reinterpret_cast<ssn_vty*>(arg);
  vty_running = true;
  while (vty_running) {
    v->poll_dispatch();
  }
  printf("Finish %s\n", __func__);
}
void vty_poll_stop() { vty_running = false; }



