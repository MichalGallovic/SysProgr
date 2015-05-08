alias ..="cd .."
alias ...="cd ../.."

alias h='cd ~'
alias c='clear'
alias vagrantown="chmod -R 755 public/ && chown -R www-data:www-data public/"
alias art="php artisan"
alias gccc="gcc -Wall -Wextra -fdiagnostics-color=auto -o"
alias defunct="ps -e -o pid,ppid,stat,cmd | grep defunct"

alias sys="cd ~/sysprogr"
alias hdr="find /usr/include | grep"
function serve() {
	if [[ "$1" && "$2" ]]
	then
		sudo dos2unix /vagrant/scripts/serve.sh
		sudo bash /vagrant/scripts/serve.sh "$1" "$2"
	else
		echo "Error: missing required parameters."
		echo "Usage: "
		echo "  serve domain path"
	fi
}

