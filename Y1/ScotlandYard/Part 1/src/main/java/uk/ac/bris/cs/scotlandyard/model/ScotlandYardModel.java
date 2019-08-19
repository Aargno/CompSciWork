package uk.ac.bris.cs.scotlandyard.model;

import static java.util.Collections.unmodifiableList;
import static java.util.Collections.unmodifiableSet;
import java.util.concurrent.CopyOnWriteArrayList;
import static java.util.Objects.requireNonNull;
import static uk.ac.bris.cs.scotlandyard.model.Colour.BLACK;
import static uk.ac.bris.cs.scotlandyard.model.Ticket.DOUBLE;
import static uk.ac.bris.cs.scotlandyard.model.Ticket.SECRET;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.ListIterator;
import java.util.Optional;
import java.util.Set;
import java.util.function.Consumer;
import uk.ac.bris.cs.gamekit.graph.Edge;
import uk.ac.bris.cs.gamekit.graph.ImmutableGraph;

import uk.ac.bris.cs.gamekit.graph.Graph;

public class ScotlandYardModel implements ScotlandYardGame, ScotlandYardView, Consumer<Move> {
    
        private List<Boolean> rounds; //List of all rounds in the current game
        private List<ScotlandYardPlayer> players; //List of all players in the current game
        private ScotlandYardPlayer currentPlayer; //Storing current player as a scorlantyardplayer object to potentially simplify some processes.
        private int currentRound = NOT_STARTED; //The round number of the current game
        private int mrXKnownLoc = -1; //MrX last known location, updated on reveal rounds -1 for unrevealed location
        private Graph<Integer, Transport> graph; //Graph acting as game map
        private List<Spectator> spectators = new CopyOnWriteArrayList<>(); //List containing all of this models spectators
        private boolean doubleMove; //Keeps track of if a double move is being made, honestly can't see a better way of doing it, but I don't like this
        private boolean rotFlag; //Keeps track of if the round rotation should continue or stop


        /**
         * Constructor to initialise ScotlandYardModel
         * 
         * @param rounds The list of rounds in the game, true indicating a reveal round
         * @param graph Graph representing the game map
         * @param mrX The player configuration for MrX
         * @param firstDetective Player configuration for the FirstDetective
         * @param restOfTheDetectives Player configurations for every other detective
         */
	public ScotlandYardModel(List<Boolean> rounds, Graph<Integer, Transport> graph,
			PlayerConfiguration mrX, PlayerConfiguration firstDetective,
			PlayerConfiguration... restOfTheDetectives) {
		this.rounds = requireNonNull(rounds);
                this.graph = requireNonNull(graph);
                if (rounds.isEmpty()) throw new IllegalArgumentException("Empty rounds");
                if (graph.isEmpty()) throw new IllegalArgumentException("Empty graph");
                if (mrX.colour.isDetective()) throw new IllegalArgumentException("MrX should be Black");
                
                ArrayList<PlayerConfiguration> configurations = new ArrayList<>();
                for (PlayerConfiguration configuration : restOfTheDetectives) 
                    configurations.add(requireNonNull(configuration));
                    configurations.add(0, firstDetective);
                    configurations.add(0, mrX);
                //checks that multiple players aren't starting in the same location
                Set<Integer> set = new HashSet<>();
                for (PlayerConfiguration configuration : configurations) {
                    if (set.contains(configuration.location)) throw new IllegalArgumentException("Duplicate location");
                    set.add(configuration.location);
                }
                //checks that there is only one of each player colour in the game
                Set<Colour> colours = new HashSet<>();
                for (PlayerConfiguration configuration : configurations) {
                    if (colours.contains(configuration.colour)) throw new IllegalArgumentException("Duplicate colour");
                    colours.add(configuration.colour);
                }
                //tests that all initial ticket values are correct
                for (PlayerConfiguration configuration : configurations) {
                    if (configuration.tickets.get(Ticket.TAXI) == null) throw new IllegalArgumentException("Players are missing tickets"); //More generalised, reduces code size, may be worth having explicit tests?
                    if (configuration.tickets.get(Ticket.BUS) == null) throw new IllegalArgumentException("Players are missing tickets");
                    if (configuration.tickets.get(Ticket.UNDERGROUND) == null) throw new IllegalArgumentException("Players are missing tickets");
                    if (configuration.tickets.get(SECRET) != 0 && configuration.colour.isDetective()) throw new IllegalArgumentException("Detectives have secret ticket");
                    if (configuration.tickets.get(DOUBLE) != 0 && configuration.colour.isDetective()) throw new IllegalArgumentException("Detectives have double ticket");
                    if(!configuration.colour.isDetective() && 
                            configuration.tickets.get(Ticket.SECRET) == null || configuration.tickets.get(Ticket.DOUBLE) == null) throw new IllegalArgumentException("MrX is missing tickets");
                }
                //create new ScotlandYardPlayers from PlayerConfiguration to store player data as they play
                players = new ArrayList<>();
                for (PlayerConfiguration configuration : configurations) 
                    players.add(new ScotlandYardPlayer(configuration.player, configuration.colour, configuration.location, configuration.tickets));
                currentPlayer = players.get(0); //Sets the initial player to be mrX
	}
        
        /**
         * Used to register spectators to the model
         * 
         * @param spectator The spectator you wish to register
         */
	@Override
	public void registerSpectator(Spectator spectator) {
		if (!spectators.contains(spectator)) spectators.add(requireNonNull(spectator));
                else throw new IllegalArgumentException("Spectator already registered");
	}
        
        /**
         * Allows you to unregister a spectator from the model, a null Spectator throws a NullPointerException
         * supplying a non-registered spectator throws an IllegalArgumentException
         * 
         * @param spectator The spectator you wish to unregister from the game model
         */
	@Override
	public void unregisterSpectator(Spectator spectator) {
                if (spectator == null) throw new NullPointerException("Null spectator provided");
                if (!spectators.contains(spectator)) throw new IllegalArgumentException("Trying to unregister non-existant spectator");
		List<Spectator> tempCopy = new CopyOnWriteArrayList<>();
                for (Spectator spec : spectators) if (spec != spectator) tempCopy.add(spec);
                spectators = tempCopy;
	}
        
        /**
         * Starts the round rotation, is called at the start of a round and for every player that plays that round
         * provided that the game hasn't ended
         */
	@Override
	public void startRotate() {
            if (currentPlayer.isMrX() && isGameOver()) throw new IllegalStateException(); //Checks that on a new round starting that the game isn't already over
            currentPlayer.player().makeMove(this, currentPlayer.location(), getValidMoves(currentPlayer), this);
        }
        
        /**
         * Called when a set of valid moves for the current player is needed by
         * the start rotate method.
         * 
         * @param player The player, who will have valid moves decided
         * @return The set of decided valid moves
         */
        private Set<Move> getValidMoves(ScotlandYardPlayer player) {
            Set<Move>validMoves = new HashSet<>();
            List<Edge<Integer, Transport>> options = new ArrayList<>(graph.getEdgesFrom(graph.getNode(player.location())));
            for (Edge<Integer, Transport> option : options) {
                if (player.hasTickets(transportConvert(option.data())) && checkDestAvail(option.destination().value()))
                    validMoves.add(new TicketMove(player.colour(), transportConvert(option.data()), option.destination().value()));
                if (player.isMrX())
                    //Makes sure player has secret tickets, and also that a secret ticket wasn't already the only way to make the move (ferry transport)
                    if (player.hasTickets(SECRET) && transportConvert(option.data()) != SECRET && checkDestAvail(option.destination().value()))
                        validMoves.add(new TicketMove(player.colour(), SECRET, option.destination().value()));
            }
            if (player.isMrX() && player.hasTickets(DOUBLE)) addValidDoubleMoves(player, validMoves);
            
            if (validMoves.isEmpty()) validMoves.add(new PassMove(player.colour()));
            
            return Collections.unmodifiableSet(validMoves);//
        }
        
        /**
         * Adds any valid double moves that can be made by the player to the moves set
         * Should only ever be run for Mr X who is always black colour, so that assumption
         * has been made
         * 
         * @param player The player we are finding moves for
         * @param moves The set that will have moves added, will be modified
         */
        private void addValidDoubleMoves(ScotlandYardPlayer player, Set<Move> moves) {
            //Used as duplicate of player, to avoid altering original player.
            ScotlandYardPlayer tempPlayer;
            List<Edge<Integer, Transport>> options = new ArrayList<>(graph.getEdgesFrom(graph.getNode(player.location())));
            for (Edge<Integer, Transport> option : options) {
                tempPlayer = new ScotlandYardPlayer(player.player(), player.colour(), player.location(), player.tickets());
                //Checks that player has requisite tickets to move once, that the destination is available, and that there are enough rounds left for a double move
                if (tempPlayer.hasTickets(transportConvert(option.data())) && checkDestAvail(option.destination().value()) && (rounds.size() - currentRound) > 1) {
                    tempPlayer.removeTicket(transportConvert(option.data()));
                    List<Edge<Integer, Transport>> options2 = new ArrayList<>(graph.getEdgesFrom(option.destination()));
                    for (Edge<Integer, Transport> option2 : options2) {
                        if (tempPlayer.hasTickets(transportConvert(option2.data())) && checkDestAvail(option2.destination().value()))
                            moves.add(new DoubleMove(BLACK, transportConvert(option.data()), option.destination().value(), 
                                                     transportConvert(option2.data()), option2.destination().value()));
                        //Runs checks to see if secret ticket moves for the given option has already been added, and that the destination is available
                        //if not, adds the permutations of the move that can be made using available secret tickets.
                        if (tempPlayer.hasTickets(SECRET) && checkDestAvail(option2.destination().value())) {
                            if (transportConvert(option.data()) != SECRET)
                                moves.add(new DoubleMove(BLACK, SECRET, option.destination().value(), 
                                                         transportConvert(option2.data()), option2.destination().value()));
                            if (transportConvert(option2.data()) != SECRET)
                                moves.add(new DoubleMove(BLACK, transportConvert(option.data()), option.destination().value(), 
                                                         SECRET, option2.destination().value()));
                            if (transportConvert(option.data()) != SECRET && transportConvert(option2.data()) != SECRET && player.hasTickets(SECRET, 2))
                                moves.add(new DoubleMove(BLACK, SECRET, option.destination().value(), 
                                                         SECRET, option2.destination().value()));
                        }
                    }
                }
            }
        }
        
        /**
         * Converts the transport type stored into the graph to the required ticket
         * 
         * @param transport The transport type we want the corresponding ticket for
         * @return Corresponding ticket for transport type provided, defaults to double as it has no associated
         * transport type.
         */
        private Ticket transportConvert(Transport transport) {
            switch (transport) {
                case BUS:
                    return Ticket.BUS;
                case TAXI:
                    return Ticket.TAXI;
                case UNDERGROUND:
                    return Ticket.UNDERGROUND;
                case FERRY:
                    return Ticket.SECRET;
                default:
                    return Ticket.DOUBLE;
            }
        }
        
        /**
         * Checks that the destination node is available for the player to move to
         * 
         * @param dest The number of the wanted destination
         * @return Returns false is detective is occupying destination, true otherwise
         */
        private boolean checkDestAvail(int dest) {
            return !players.stream().anyMatch((player) -> (player.location() == dest && !player.isMrX())); //Alternate functional implementation
        }
        
        /**
         * Gets an immutable list of all the models spectators
         * 
         * @return Immutable list of all spectators
         */
	@Override
	public Collection<Spectator> getSpectators() {
            return unmodifiableList(spectators);
	}
        
        /**
         * Returns an immutable list of the colours of all the players
         * 
         * @return Immutable list of player colours
         */
	@Override
	public List<Colour> getPlayers() {
		List<Colour> tempCol = new ArrayList<>();
                for (ScotlandYardPlayer player : players) tempCol.add(player.colour());
                return unmodifiableList(tempCol);
	}
        
        /**
         * Returns an immutable set of the winning players, either detectives, MrX
         * or an empty set
         * 
         * @return Immutable set containing winning players
         */
	@Override
	public Set<Colour> getWinningPlayers() {
               Set<Colour> tempColour = new HashSet<>();
               if (checkDetectivesLose()) tempColour.add(BLACK);
               //adds all detectives to winning players if detectives win
               else if (checkMrXLose()) players.stream().filter((player) -> !(player.isMrX())).forEach((player) -> {
                   tempColour.add(player.colour());
               });
               return unmodifiableSet(tempColour);
	}
        
        /**
         * Gets the current location of the player with the specified colour, but for MrX,
         * unless the current round is a reveal round, only his last known location
         * will be provided
         * 
         * @param colour The colour of the player you want the location of
         * @return Either the players current location, MrX's last known location, or an empty for an invalid player
         */
	@Override
	public Optional<Integer> getPlayerLocation(Colour colour) {
                if (colour == BLACK && mrXKnownLoc == -1) return Optional.of(0);
                else if (colour == BLACK) return Optional.of(mrXKnownLoc);
                else for (ScotlandYardPlayer player : players) 
                    if (player.colour() == colour) return Optional.of(player.location());
                return Optional.empty();
	}
        
        /**
         * Gets the number of provided ticket type, held by player with provided colour.
         * 
         * @param colour Colour of the player to check
         * @param ticket Ticket type to check
         * @return Returns the number of the specific type of ticket held by the requested player, or an empty if the provided player doesn't exist.
         */
	@Override
	public Optional<Integer> getPlayerTickets(Colour colour, Ticket ticket) {
		for (ScotlandYardPlayer player : players) 
                    if (player.colour() == colour) return Optional.of(player.tickets().get(ticket));
                return Optional.empty();
	}
        
        /**
         * Checks if the game on the model is over
         * 
         * @return Boolean stating if the game is over or not
         */
	@Override
	public boolean isGameOver() {
            return (checkMrXLose() || checkDetectivesLose());
	}
        
        /**
         * Checks if MrX has lost the game
         * 
         * @return Boolean storing if MrX has lost or not
         */
        private boolean checkMrXLose() {
            ScotlandYardPlayer mrX = players.get(0);
            if (getValidMoves(mrX).contains(new PassMove(BLACK))) return true; //Surrounded (pass move for MrX means MrX is stuck)
            return checkMrXCaught();
        }
        
        /**
         * Separate method to allow for re-usage. Allows us to check if the game/round
         * should end early because MrX has been caught.
         * 
         * @return Boolean saying if MrX has been caught
         */
        private boolean checkMrXCaught() {
            ScotlandYardPlayer mrX = players.get(0);
            return players.stream().anyMatch((player) -> (!player.isMrX() && player.location() == mrX.location()));
        }
        
        /**
         * Checks if detectives have lost game
         * 
         * @return Boolean storing if detectives have lost
         */
        private boolean checkDetectivesLose() {
            //Ensures all elements satisfy condition
            if (players.stream().filter((player) -> (!player.isMrX())).allMatch((player) -> (getValidMoves(player).contains(new PassMove(player.colour())))) && currentPlayer.isMrX()) return true;
            return (currentRound == rounds.size() && currentPlayer.isMrX()); //Checks if end game has been reached
        }
        
        /**
         * Gets the colour of the models current player
         * 
         * @return Colours of the current player
         */
	@Override
	public Colour getCurrentPlayer() {
		return currentPlayer.colour();
	}
        
        /**
         * Gets the current round of the game
         * 
         * @return Current round of the game
         */
	@Override
	public int getCurrentRound() {
		return currentRound;
	}
        
        /**
         * Gets an immutable list of all the rounds in the game
         * 
         * @return Immutable list of game rounds
         */
	@Override
	public List<Boolean> getRounds() {
		return unmodifiableList(rounds);
	}
        
        /**
         * Gets the graph acting as a game map, and returns as an immutable graph
         * 
         * @return Immutable graph containing graph serving as game map
         */
	@Override
	public Graph<Integer, Transport> getGraph() {
		return new ImmutableGraph<>(graph);
	}
        
        /**
         * Accept method that is implemented as part of a Consumer interface.
         * Called by a ScotlandYardPlayers make move method. Starts processing
         * the move provided by the player, and then continues to the next player.
         * Continues until MrX is reached as the next player or the game ends for any reason.
         * 
         * @param move The move to be processed for the current player
         */
        @Override
        public void accept(Move move) {
            if (doubleMove) currentPlayer = players.get(0);
            if(move == null) throw new NullPointerException("Callback move is null");
            if (!getValidMoves(currentPlayer).contains(move)) throw new IllegalArgumentException("Illegal move provided");
            boolean doubleUsed;
            //Get the number of double tickets held by MrX at the start of the round
            int doubleTick = players.get(0).tickets().get(DOUBLE);
            //move logic
            handleMove(move);
            rotFlag = nextPlayer();
            doubleUsed = doubleTicketUsed(doubleTick);
            //only iterates currentRound here if a double move was not taken; round incrementation for double moves is handled seperately
            if (players.indexOf(currentPlayer) == 1 && !doubleUsed) currentRound++;
            if (!doubleUsed) notifications(move, rotFlag);
            //Only rotates if all players played, and it's not a DoubleMove
            if (rotFlag && !doubleMove && !checkMrXCaught()) startRotate();
            //Resets currentPlayer to MrX at end of doubleMove call to allow simple transition to either next part of double move, or next players move
            else if (doubleMove) currentPlayer = players.get(0);
        }
        
        /**
         * Handles move logic / behaviour
         * 
         * @param move The move to be used
         */
        private void handleMove(Move move) {
            MoveVisitor visit = new LogicVisitor(currentPlayer, this);
            move.visit(visit);
        }
        
        /**
         * Method to handle getting next player
         * 
         * @return whether rotation should end or not
         */
        boolean nextPlayer() {
            ListIterator<ScotlandYardPlayer> player = players.listIterator(players.indexOf(currentPlayer) + 1);
            if (doubleMove) {
                currentPlayer = players.get(1);
                return false;
            }
            else if(player.hasNext()) currentPlayer = player.next();
            else {
                currentPlayer = players.get(0);
                return false;
            }
            return true;
        }
        
        /**
         * Provides all needed notifications to spectators
         * 
         * @param move The move thats being made
         * @param rotFlag False if rotation is ending
         */
        private void notifications(Move move, boolean rotFlag) {
            if (currentPlayer == players.get(1)) spectators.forEach((s) -> s.onRoundStarted(this, currentRound));
            //only calls onMoveMade from here if Mr. X is on a reveal round or the current player is a detective; hidden round onMoveMade calls are handled seperately
            if (rounds.get(currentRound - 1) || !move.colour().equals(BLACK)) spectators.forEach((s) -> s.onMoveMade(this, move));
            //handles onMoveMade calls for hidden rounds
            else move.visit(new MoveHideVisitor(this));
            if ((!rotFlag && isGameOver()) || checkMrXCaught()) spectators.forEach((s) -> s.onGameOver(this, getWinningPlayers()));
            else if (!rotFlag && !doubleMove) spectators.forEach((s) -> s.onRotationComplete(this));
        }
        
        /**
         * Checks if double tickets were used in a turn
         * 
         * @param doubleTick The number of double tickets held at the start of the turn
         * @return 
         */
        private boolean doubleTicketUsed(int doubleTick) {
            //Checks if MrX has lost a double move ticket since the start of the turn
            return (doubleTick != players.get(0).tickets().get(DOUBLE));
        }
       
        /**
         * Specific notification for a move, used by MoveVistors
         * 
         * @param move The move to be used in the notifications
         */
        protected void moveNotify(Move move) {
            spectators.forEach((s) -> s.onMoveMade(this, move));
        }
        
        /**
         * Used to transfer a detectives used ticket to MrX
         * 
         * @param tick The ticket to be transferred
         */
        protected void transferMrXTicket(Ticket tick) {
            players.get(0).addTicket(tick);
        }
        
        /**
         * Sets class variable indicating that a doubleMove is being made
         * 
         * @param flag The value to update the doubleMove flag to
         */
        protected void doubleMoveUpdate(boolean flag) {
            doubleMove = flag;
        }
        
        /**
         * Updates MrX's known location on reveal rounds
         * 
         * @param location The location to set MrX's last known location to
         */
        protected void updateMrXKnownLoc(int location) {
            mrXKnownLoc = location;
        }  

}
