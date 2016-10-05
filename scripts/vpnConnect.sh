#!/bin/bash
#
# Connect to cubesat gateway via vpn
#
keyDir=~/.ssh
sshConfig=$keyDir/config
commonName=$(whoami)
cfgFile=~/.irvine-01.keyInfo
instdir=$(cd $(dirname $0); pwd)
ovpnCfg=$(mktemp)

keytool=$instdir/opensslKeyTool.sh

cleanup()
{
    rm -f $ovpnCfg
}

trap cleanup EXIT

log()
{
    echo "$*"
}

usage()
{
    cat <<EOF
Usage:  $0 [options] 

        Tool to connect to openvpn

Options
    -c {ovpnx file} Connect to VPN using the ovpnx file
    -f {cfgFile}    File to store cert params/initialze params (default $cfgFile)
    -u              Update the config file with the latest settings (existing will be renamed config.bak)
    -h              this message
EOF
    exit 1
}


while getopts "c:uf:h" arg; do
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
    . "$cfgFile"
else
    log "[E] $cfgFile not found.  Generate your key info with opensslKeyTool.sh"
    usage
fi

cert=$keyDir/${keyName}.cert
privKey=$keyDir/${keyName}.key
pubKey=$keyDir/${privKey}.pub

#
# Set up ssh config
#
setupSshConfig()
{
    chmod 700 $keyDir
    cat <<EOF 
Setting up default ssh config...  Once you connect, you should be able to
connect to irvine-01 via "ssh irvine-01" and cubesat gateway via
"ssh cubesatgateway"
EOF
    cat >$sshConfig <<EOF
Host cubesatgateway
     HostName 10.133.33.2
     User vpnuser
     IdentityFile ${privKey}

Host irvine-01
  User root
  ProxyCommand ssh -q cubesatgateway nc 192.168.0.100 22
  
EOF
    chmod 600 $sshConfig
    log "[I] Updated $sshConfig: "
    cat $sshConfig
}

case $cmd in
    "connect")
        if [ ! -f $sshConfig ]; then
            setupSshConfig
        fi
        if [ ! -f "${pubKey}" ]; then
            $keytool -p -f $cfgFile
            if [ $? -ne 0 ]; then
                log "[E] Unable to generate public key from $cert"
                exit 1
            fi
        fi

        #
        # Some systems like Fedora don't have nogroup, so replace it with nobody
        #
        if id -u nogroup >& /dev/null; then
            $keytool -d $ovpnx -o $ovpnCfg -f $cfgFile
        else
            $keytool -d $ovpnx -o /dev/stdout -f $cfgFile |sed -e "s/nogroup/nobody/g">$ovpnCfg
        fi
        if [ $? -ne 0 ]; then
            log "[E] Unable to decode openvpn configuration"
            exit 1
        fi
        sudo openvpn --config "${ovpnCfg}"
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
