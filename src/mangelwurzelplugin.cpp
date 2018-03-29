/**
 * @file mangelwurzelplugin.cpp Plug-in wrapper for the mangelwurzel sproutlet.
 *
 * Copyright (C) Metaswitch Networks 2016
 * If license terms are provided to you in a COPYING file in the root directory
 * of the source code repository by which you are accessing this code, then
 * the license outlined in that COPYING file applies to your use.
 * Otherwise no rights are granted except for those provided to you by
 * Metaswitch Networks in a separate written agreement.
 */

#include "cfgoptions.h"
#include "cfgoptions_helper.h"
#include "acr.h"
#include "sproutletplugin.h"
#include "mangelwurzel.h"

class MangelwurzelPlugin : public SproutletPlugin
{
public:
  MangelwurzelPlugin();
  ~MangelwurzelPlugin();

  bool load(struct options& opt, std::list<Sproutlet*>& sproutlets);
  void unload();

private:
  Mangelwurzel* _mangelwurzel;
};

/// Export the plug-in using the magic symbol "sproutlet_plugin"
extern "C" {
MangelwurzelPlugin sproutlet_plugin;
}

MangelwurzelPlugin::MangelwurzelPlugin() :
  _mangelwurzel(NULL)
{
}

MangelwurzelPlugin::~MangelwurzelPlugin()
{
}

/// Loads the mangelwurzel plug-in, returning the supported Sproutlets.
bool MangelwurzelPlugin::load(struct options& opt, std::list<Sproutlet*>& sproutlets)
{
  bool plugin_loaded = true;

  std::string mangelwurzel_prefix = "mangelwurzel";
  int mangelwurzel_port = 0;
  std::string mangelwurzel_uri = "";
  bool mangelwurzel_enabled = true;
  std::string plugin_name = "mangelwurzel-as";

  std::map<std::string, std::multimap<std::string, std::string>>::iterator
    mangelwurzel_it = opt.plugin_options.find(plugin_name);

  if (mangelwurzel_it == opt.plugin_options.end())
  {
    TRC_STATUS("Mangelwurzel options not specified on Sprout command. Mangelwurzel disabled.");
    mangelwurzel_enabled = false;
  }
  else
  {
    TRC_DEBUG("Got Mangelwurzel options map");
    std::multimap<std::string, std::string>& mangelwurzel_opts = mangelwurzel_it->second;

    set_plugin_opt_int(mangelwurzel_opts,
                       "mangelwurzel",
                       "mangelwurzel-as",
                       true,
                       mangelwurzel_port,
                       mangelwurzel_enabled);

    if (mangelwurzel_port < 0)
    {
      TRC_STATUS("Mangelwurzel port set to a value of less than zero (%d). Disabling mangelwurzel.",
                 mangelwurzel_port);
      mangelwurzel_enabled = false;
    }

    set_plugin_opt_str(mangelwurzel_opts,
                       "mangelwurzel_prefix",
                       "mangelwurzel-as",
                       false,
                       mangelwurzel_prefix,
                       mangelwurzel_enabled);

    // Given the prefix, set the default uri
    mangelwurzel_uri = "sip:" + mangelwurzel_prefix + "." + opt.sprout_hostname + ";transport=TCP";

    set_plugin_opt_str(mangelwurzel_opts,
                       "mangelwurzel_uri",
                       "mangelwurzel-as",
                       false,
                       mangelwurzel_uri,
                       mangelwurzel_enabled);
  }

  if (mangelwurzel_enabled)
  {
    // Create the Sproutlet.
    _mangelwurzel = new Mangelwurzel(mangelwurzel_prefix,
                                     mangelwurzel_port,
                                     mangelwurzel_uri);

    sproutlets.push_back(_mangelwurzel);
  }

  return plugin_loaded;
}

/// Unloads the mangelwurzel plug-in.
void MangelwurzelPlugin::unload()
{
  delete _mangelwurzel;
}
