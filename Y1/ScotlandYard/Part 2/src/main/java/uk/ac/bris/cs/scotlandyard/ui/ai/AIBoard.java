/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package uk.ac.bris.cs.scotlandyard.ui.ai;

import java.util.ArrayList;
import java.util.Collections;
import static java.util.Collections.unmodifiableSet;
import java.util.HashSet;
import java.util.List;
import java.util.Optional;
import java.util.Set;
import uk.ac.bris.cs.gamekit.graph.Edge;
import uk.ac.bris.cs.gamekit.graph.Graph;
import uk.ac.bris.cs.scotlandyard.model.Colour;
import static uk.ac.bris.cs.scotlandyard.model.Colour.BLACK;
import uk.ac.bris.cs.scotlandyard.model.DoubleMove;
import uk.ac.bris.cs.scotlandyard.model.Move;
import uk.ac.bris.cs.scotlandyard.model.PassMove;
import uk.ac.bris.cs.scotlandyard.model.Ticket;
import static uk.ac.bris.cs.scotlandyard.model.Ticket.DOUBLE;
import static uk.ac.bris.cs.scotlandyard.model.Ticket.SECRET;
import uk.ac.bris.cs.scotlandyard.model.TicketMove;
import uk.ac.bris.cs.scotlandyard.model.Transport;

/**
 * Stores information about a given board state
 * 
 * @author Limited
 */
public class AIBoard {
    
        private List<AIPlayer> players; //List of all players on the board
        private int currentRound; //The current round fo the game being played
        private List<Boolean> rounds; //List of all rounds in the current game
        private Optional<Move> move = Optional.empty(); //Can contain the move to be made on the board
        private Colour currentPlayer; //Stores colour of current player
        private Graph<Integer, Transport> graph; //Graph acting as game map
        
        /**
         * Constructor for AIBoard
         * 
         * @param players List of players playing on the board
         * @param currentRound The round number the board should start from
         * @param rounds The list of rounds in the game
         * @param move The move being made on the constructed board
         * @param currentPlayer The colour of the current player
         * @param graph A graph representing the map used in the game
         */
        public AIBoard(List<AIPlayer> players, int currentRound, List<Boolean> rounds, Move move, Colour currentPlayer, Graph<Integer, Transport> graph) {
            this.players = players;   
            this.currentRound = currentRound;
            this.rounds = rounds;
            this.move = Optional.of(move);
            this.currentPlayer = currentPlayer;
            this.graph = graph;
        }
        
        /**
         * Alternate constructor without a Move; for use in constructing an AIBoard to be used as the root of a gameTree
         * 
         * @param players List of players playing on the board
         * @param currentRound The round number the board should start from
         * @param rounds The list of rounds in the game
         * @param currentPlayer The colour of the current player
         * @param graph A graph representing the map used in the game
         */
        public AIBoard(List<AIPlayer> players, int currentRound, List<Boolean> rounds, Colour currentPlayer, Graph<Integer, Transport> graph) {
            this.players = players;   
            this.currentRound = currentRound;
            this.rounds = rounds;
            this.currentPlayer = currentPlayer;
            this.graph = graph;
        }

        /**
         * Retrieves MrX's location
         * 
         * @return the mrXLocation
         */
        public int getMrXLocation() {
            return players.get(0).location();
        }
        
        /**
         * Retrieves the AIPlayer object containing the colour matching the provided colour
         * if it exits. Otherwise supplies and empty.
         * 
         * @param colour The colour of the player to be searched for
         * @return Either the player, if a player with a matching colour exits, or an empty if a player with a matching colour doesn't exits.
         */
        public Optional<AIPlayer> getPlayer(Colour colour) {
            for (AIPlayer player : players) {
                if (player.colour() == colour) return Optional.of(player);
            }
            return Optional.empty();
        }

        /**
         * Retrieves all player locations apart from MrX
         * 
         * @return the detective locations
         */
        public List<Integer> getPlayerLocations() {
            ArrayList<Integer> playerLocs = new ArrayList<>();
            for (AIPlayer player : players) {
                if (! player.isMrX()) playerLocs.add(player.location());
            }
            return playerLocs;
        }
        
         /**
          * Returns a list of all players in the game, MrX and detectives
          * 
         * @return the list of players
         */
        public List<AIPlayer> getPlayers() {
                return players;
        }
        
        /**
         * Retrieves a list of all detectives in the game
         * 
         * @return the list of detectives
         */
        public List<AIPlayer> getDetectives() {
                ArrayList<AIPlayer> detectives = new ArrayList<>();
                for (AIPlayer player : players) {
                    if (! player.isMrX()) detectives.add(player);
                }
                return detectives;
        }
        
        
        /**
         * Returns the AIPlayer object for MrX
         * 
         * @return MrX's player
         */
        public AIPlayer getMrX() {
                return players.get(0);
        }
        
        

        /**
         * Returns the number of the current round
         * 
         * @return the currentRound
         */
        public int getCurrentRound() {
            return currentRound;
        }
        
        /**
         * Used to update the current round to a provided value
         * 
         * @param round The new value for the boards current round
         */
        public void setCurrentRound(int round) {
            currentRound = round;
        }

        /**
         * Returns a list containing the rounds to be used in this board
         * 
         * @return List of all rounds to be used in board
         */
        public List<Boolean> getRounds() {
            return rounds;
        }

        /**
         * Gets the move associated with this board
         * 
         * @return The move associated with the board
         */
        public Optional<Move> getMove() {
            return move;
        }

        /**
         * Gets the colour of the current player on the board
         * 
         * @return The colour of the current player
         */
        public Colour getCurrentPlayer() {
            return currentPlayer;
        }
        
        /**
         * Retires the current players associated AIPlayer object
         * 
         * @return The AIPlayer object with the same colour as is stored in currentPlayer
         */
        public Optional<AIPlayer> getCurrentPlayerAsAIPlayer() {
            for (AIPlayer player : players) {
                if (player.colour() == currentPlayer) return Optional.of(player);
            }
            return Optional.empty();
        }
        
        /**
         * Sets the currentPlayer colour to the colour of the AIPlayer stored by this boards
         * player lists at the index provided.
         * 
         * @param index The location in the players list of the desired new current player
         */
        public void setCurrentPlayer(int index) {
            currentPlayer = players.get(index).colour();
        }
        
        /**
         * Converts the data stored on this AIBoard object to a string, which can then be used
         * for various purposes.
         * 
         * @return The string representation of the data stored in this object.
         */
        @Override
	public String toString() {
		final StringBuilder sb = new StringBuilder("AIPlayer{");
		sb.append(", player locations=").append(getPlayerLocations());
		sb.append(", current player=").append(getCurrentPlayer());
		sb.append(", current round=").append(getCurrentRound());
                sb.append(", move=").append(getMove());
		sb.append('}');
		return sb.toString();
	}
        
        /**
         * Returns an immutable set containing all of the winning players on the board
         * in it's current state
         * 
         * @return Immutable set containing all winning players.
         */
	public Set<Colour> getWinningPlayers() {
               Set<Colour> tempColour = new HashSet<>();
               if (checkDetectivesLose()) tempColour.add(BLACK);
               else if (checkMrXLose()) players.stream().filter((player) -> !(player.isMrX())).forEach((player) -> {
                   tempColour.add(player.colour());
               });
               return unmodifiableSet(tempColour);
	}
        
        /**
         * Checks if MrX has lost the game in the boards current state.
         * 
         * @return Boolean indicating if MrX has lost
         */
        public boolean checkMrXLose() {
            AIPlayer mrX = players.get(0);
            //Surrounded (pass move for MrX means MrX is stuck)
            if (getValidMoves(mrX).contains(new PassMove(BLACK))) return true;
            return checkMrXCaught();
        }
        
        /**
         * Separate method to allow for re-usage. Allows us to check if the game/round
         * should end early because MrX has been caught.
         * 
         * @return Boolean saying if MrX has been caught
         */
        private boolean checkMrXCaught() {
            AIPlayer mrX = getMrX();
            return players.stream().anyMatch((player) -> (!player.isMrX() && player.location() == mrX.location()));
        }
        
        /**
         * Checks if detectives have lost game
         * 
         * @return Boolean storing if detectives have lost
         */
        public boolean checkDetectivesLose() {
            //Ensures all elements satisfy condition
            if (players.stream().filter((player) -> (!player.isMrX())).allMatch((player) -> (getValidMoves(player).contains(new PassMove(player.colour())))) && currentPlayer.isMrX()) return true;
            return (currentRound == rounds.size() && currentPlayer.isMrX()); //Checks if end game has been reached
        }

        /**
         * Called to generate the set of valid moves available to the current player
         * 
         * @return The set of decided valid moves
         */
        public Set<Move> getValidMoves() {
                Set<Move>validMoves = new HashSet<>();
                players = this.getPlayers();
                AIPlayer player;
                if (this.getCurrentPlayerAsAIPlayer().isPresent()) {
                    player = this.getCurrentPlayerAsAIPlayer().get();
                }
                else throw new IllegalArgumentException("AIBoard has no current player (somehow)");
                                
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
         * Called to generate all of the valid moves available to the player specified
         * 
         * @param player The player, who will have valid moves decided
         * @return The set of decided valid moves
         */
        public Set<Move> getValidMoves(AIPlayer player) {
                Set<Move>validMoves = new HashSet<>();
                players = this.getPlayers();
                                
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
        private void addValidDoubleMoves(AIPlayer player, Set<Move> moves) {
                AIPlayer tempPlayer; //Used as duplicate of player, to avoid altering original player.
                List<Edge<Integer, Transport>> options = new ArrayList<>(graph.getEdgesFrom(graph.getNode(player.location())));
                for (Edge<Integer, Transport> option : options) {
                    tempPlayer = new AIPlayer(player.colour(), player.location(), player.tickets());
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
                return !players.stream().anyMatch((player) -> (player.location() == dest && !player.isMrX())); 
        }       
}
