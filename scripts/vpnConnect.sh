#!/bin/bash
#
# Connect to cubesat gateway via vpn
#

#
# 
#
connectMessage()
{
    cat <<EOF 
#########################################################################
           You are connected to Irvine Cubesat Net!

In a different terminal window type:

"ssh irvine-02"       to connect to irvine-02
"ssh cubesatgateway"  to connect to cubesat gateway
"ssh irvine-ops"      to connect to the irvine cubesat groundstation @ Cal Poly

Please refer to http://doc.irvinecubesat.com/display/HD/Troubleshooting+ssh+Connection+Issues 
if you have problems with ssh.
#########################################################################
EOF
}

#
# Print the connect message without having to initialize other variables below.
#
if [ "$1" = "-m" ]; then
    connectMessage
    exit 0
fi

keyDir=~/.ssh
sshConfig=$keyDir/config
oldCfgFile=~/.irvine-01.keyInfo
cfgFile=~/.irvinecubesat.keyInfo
instdir=$(cd "$(dirname "$0")"&&pwd)
ovpnCfg=$(mktemp --tmpdir="${HOME}"/.ssh/)
ovpnx=$(find ~/.ssh/ -name "*.ovpnx"|head -1)
cmd="connect"
keytool=$instdir/opensslKeyTool.sh

cleanup()
{
    rm -f "$ovpnCfg"
}

trap cleanup EXIT

log()
{
    echo "$*"
}

if [ -e "$oldCfgFile" ]; then
    log "[I] Converting $oldCfgFile to $cfgFile"
    
    if ! mv "$oldCfgFile" "$cfgFile"; then
	log "[E] Unable to ove $oldCfgFile to  $cfgFile"
    fi
fi
usage()
{
    cat <<EOF
Usage:  $0 [options] 

        Tool to connect to openvpn

Options
    -c {ovpn/x file} Connect to VPN using the ovpnx or ovpn file (default $ovpnx)
    -f {cfgFile}    File to store cert params/initialze params (default $cfgFile)
    -u              Update the config file with the latest settings (existing will be renamed config.bak)
    -h              this message
EOF
    exit 1
}

while getopts "c:uf:mh" arg; do
    case $arg in
        c)
            cmd="connect"
            ovpnx=$OPTARG
            ;;
        f)
            cfgFile=$OPTARG
            ;;
        u)
            cmd="update"
            ;;
        m)
            connectMessage
            exit 0
            ;;
        h)
            usage
            ;;
        *)
            usage
            ;;
    esac
done
shift "$((OPTIND-1))"

if [ -n "$cfgFile" ] && [ -f "$cfgFile" ]; then
    # shellcheck disable=SC1090
    . "$cfgFile"
else
    log "[E] $cfgFile not found.  Generate your key info with opensslKeyTool.sh"
    usage
fi

if [ ! -e "$ovpnx" ]; then
    log "[E] $ovpnx does not exist"
    usage
fi

# shellcheck disable=SC2154
cert=$keyDir/"${keyName}.cert"
# shellcheck disable=SC2154
privKey=$keyDir/"${keyName}.key"
pubKey=$keyDir/"${privKey}.pub"

#
# Set up ssh config
#
setupSshConfig()
{
    chmod 700 "$keyDir"
    chmod 700 "$keyDir/*.key"
    cat >$sshConfig <<EOF
Host cubesatgateway
     HostName 10.133.33.2
     User vpnuser
     IdentityFile ${privKey}

Host irvine-02
     User root
     ProxyCommand ssh -q cubesatgateway nc 192.168.0.101 22
  
Host irvine-ops
     HostName cslvm229.csc.calpoly.edu
     User irvine-ops
     IdentityFile ${privKey}
EOF
    chmod 600 "$sshConfig"
    log "[I] Updated $sshConfig: "
    cat "$sshConfig"
}

case $cmd in
    "connect")
        if [ ! -f "$sshConfig" ]; then
            setupSshConfig
        fi
        if [ ! -f "${pubKey}" ]; then
            
            if ! $keytool -p -f "$cfgFile"; then
                log "[E] Unable to generate public key from $cert"
                exit 1
            fi
        fi

        NOBODY_SED="sed -e s/nogroup/nobody/g"
        
        hasNoGroup()
        {
            grep -q nogroup /etc/group
        }
        
        decryptCfg()
        {
            #
            # Some systems like Fedora don't have nogroup, so replace it with nobody
            #
            if hasNoGroup; then
                $keytool -d "$ovpnx" -o "$ovpnCfg" -f "$cfgFile"
            else
                $keytool -d "$ovpnx" -o /dev/stdout -f "$cfgFile" |$NOBODY_SED>"$ovpnCfg"
            fi
            keytoolStatus=$?

            if [ $keytoolStatus -ne 0 ]; then
                log "[E] Unable to decode openvpn configuration"
                exit 1
            fi
        }
        copyCfg()
        {
            if hasNoGroup; then
                cp "$ovpnx" "$ovpnCfg"
            else
                $NOBODY_SED "$ovpnx" >"$ovpnCfg"
            fi
        }
        if [ "${ovpnx##*.}" = "ovpnx" ]; then
            decryptCfg
        else
            copyCfg
        fi
        
        sudo openvpn --config "${ovpnCfg}" --script-security 2 --up "$0 -m"
        ;;
    "update")
        if [ -e "$sshConfig" ]; then
            log "[I] Backing up $sshConfig to $sshConfig.bak"
            cp --remove-destination "$sshConfig"{,.bak}
        fi
        setupSshConfig
        ;;
    *)
        log "[E] Unknown command:  $cmd"
        usage
esac
