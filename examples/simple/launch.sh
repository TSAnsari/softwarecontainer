#!/bin/sh

# Copyright (C) 2016 Pelagicore AB
#
# Permission to use, copy, modify, and/or distribute this software for
# any purpose with or without fee is hereby granted, provided that the
# above copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
# WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR
# BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES
# OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
# WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
# ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
# SOFTWARE.
#
# For further information see LICENSE


# This is an example of a misbehaving application being launched in a
# container, the example will launch simple.c in a container, and observe that
# it is killed inside the container by the OOM killer.

# Register the path of the script. This is what we will mount in to the
# container later, in order to access the 'simple' binary.
SCRIPTPATH=$( cd $(dirname $0) ; pwd -P )

help() {
    echo "$0 [-b [system|session]]"
    echo ""
    echo "  -b    what type of dbus to use, session or system"
    exit
}


BUS="system"
while getopts ":b:h" opt; do
    case $opt in
    b)
        BUS="$OPTARG"
        ;;
    h)
        help
        ;;
    *)
        echo "Unknown option $OPTARG"
        help
    esac
done

# Create a new session D-Bus bus
eval `dbus-launch --sh-syntax`

# Print the environment
env

# Launch a new agent
softwarecontainer-agent &
AGENTPID="$!"

# Let the agent start up
sleep 2

# Destination
SCNAME="com.pelagicore.SoftwareContainerAgent"
# Object path
SCOBJPATH="/com/pelagicore/SoftwareContainerAgent"
# Prefix for dbus methods
AGENTPREFIX="com.pelagicore.SoftwareContainerAgent"
export SC_CMD="dbus-send --${BUS} --print-reply --dest=$SCNAME $SCOBJPATH"

# Introspect the agent
$SC_CMD org.freedesktop.DBus.Introspectable.Introspect

# Ping the agent
$SC_CMD $AGENTPREFIX.Ping

# Create a new container
$SC_CMD $AGENTPREFIX.CreateContainer string:'[{"writeOften": "0"}]'

# A few thing that we use for more or less every call below
CONTAINERID="uint32:0"
ROOTID="uint32:0"
OUTFILE="/tmp/stdout"

# Expose a directory to the container
$SC_CMD $AGENTPREFIX.BindMountFolderInContainer \
    $CONTAINERID \
    string:${SCRIPTPATH} \
    string:app \
    boolean:true
APPBASE="/gateways/app"

# Run the simple example
$SC_CMD $AGENTPREFIX.LaunchCommand \
    $CONTAINERID \
    $ROOTID \
    string:$APPBASE/simple \
    string:$APPBASE \
    string:$OUTFILE \
    dict:string:string:""

# Let the example run for a while
sleep 30

# Clean up
kill $AGENTPID