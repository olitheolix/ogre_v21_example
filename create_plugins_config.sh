#!/bin/bash

fname=plugins.cfg

echo "# --- Define plugin folder" > $fname
echo "PluginFolder="$CONDA_PREFIX"/lib/OGRE" >> $fname
echo "" >> $fname
echo "# --- Define plugins" >> $fname
echo "" >> $fname
echo "# Plugin=RenderSystem_Direct3D11" >> $fname
echo "Plugin=RenderSystem_GL3Plus" >> $fname
