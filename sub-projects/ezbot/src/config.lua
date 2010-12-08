-- config file

network = "irc.oftc.net"   -- network to connect (ezbox default IRC server)

-- The nick list is useful. If the first nick is already taken, will try with the other nicks.
-- So, be sure to add a lot of nicks.
nickList = {
            "EzBot",
            "EzBot[1]",
            "EzBot[2]",
            "EzBot[3]",
            "EzBot[4]",
            "EzBot[5]",
            "EzBot[6]",
            "EzBot[7]",
            "EzBot[8]",
            "EzBot[9]"
            }
defaulChannels = {"#ezbox-cn"} --channels to join on startup
username = "EzBot"
realname = "EzBot"

---------------------------
-- Add plugins here!
---------------------------
require('plugin.google')
require('plugin.query')
require('plugin.calc')
require('plugin.translate')
require('plugin.join')
require('plugin.part')
require('plugin.weather')
require('plugin.shortcommand')


