/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package uk.ac.bris.cs.scotlandyard.model;

/**
 * VVisitor for move objects, handles processing of moves.
 * 
 * @author robey
 */
public class LogicVisitor implements MoveVisitor{

    private ScotlandYardPlayer player;
    private ScotlandYardModel model;
    
    /**
     * Constructor for the move visitor, takes the game model, and the player making
     * a move as arguments to process the changes made by moves.
     * 
     * @param player The player making a move
     * @param model The game model the move is being made on
     */
    public LogicVisitor(ScotlandYardPlayer player, ScotlandYardModel model) {
        this.player = player;
        this.model = model;
    }
    
    /**
     * Updates player location and model details using provided pass move
     * 
     * @param move The pass move that is being visited
     */
    @Override
    public void visit(PassMove move) {
        //Does nothing, as move is just passed
    }
    
    /**
     * Updates player location and model details using provided ticket move
     * 
     * @param move The ticket move that is being visited
     */
    @Override
    public void visit(TicketMove move) {
        player.location(move.destination());
        player.removeTicket(move.ticket());
        if(!player.isMrX()) model.transferMrXTicket(move.ticket());
        if (player.isMrX() && model.getRounds().get(model.getCurrentRound())) model.updateMrXKnownLoc(move.destination());
    }

    /**
     * Updates player location and model details using provided double move,
     * also will notify spectators of a double move occurring before handling the individual moves.
     * 
     * @param move The double move to be processed
     */
    @Override
    public void visit(DoubleMove move) {
        model.doubleMoveUpdate(true);
        player.removeTicket(Ticket.DOUBLE);
        //Moves model to next player early for spectator updates
        model.nextPlayer();
        //Hides details of double move
        move.visit(new MoveHideVisitor(model));
        model.accept(move.firstMove());
        model.accept(move.secondMove());
        model.doubleMoveUpdate(false);
    }

}
